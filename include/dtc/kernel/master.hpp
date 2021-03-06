/******************************************************************************
 *                                                                            *
 * Copyright (c) 2017, Tsung-Wei Huang, Chun-Xun Lin, and Martin D. F. Wong,  *
 * University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
 *                                                                            *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * This program is free software. You can redistribute and/or modify          *
 * it in accordance with the terms of the accompanying license agreement.     *
 * See LICENSE in the top-level directory for details.                        *
 *                                                                            *
 ******************************************************************************/

#ifndef DTC_KERNEL_MASTER_HPP_
#define DTC_KERNEL_MASTER_HPP_

#include <dtc/kernel/scheduler.hpp>
#include <dtc/kernel/manager.hpp>

namespace dtc {

// Class: Master
class Master : public KernelBase {

  // Webui-specific structures.

  struct AgentInfo {
    const key_type key;
    pb::Resource resource;
    pb::Resource released;
    size_t num_tasks;
    json to_json() const;
  };

  struct MasterInfo {
    std::string host;
    size_t num_agents;
    size_t num_graphs; 
    json to_json() const;
  };

  struct ClusterInfo {
    MasterInfo master;
    std::vector<AgentInfo> agents;
    json to_json() const;
  };


  // ---- Actor definitions.

  // Agent actor.
  struct Agent : ActorBase {
    
    const key_type key;
    
    std::optional<pb::Resource> resource; 
    std::optional<pb::Resource> released;

    struct TaskMeta {
      const pb::Topology topology;
    };

    std::unordered_map<TaskID, TaskMeta> taskmeta;

    Agent(key_type k) : key {k} {}
  };
  
  // Graph actor.
  struct Graph : ActorBase {
    
    const key_type key;

    std::optional<pb::Topology> topology;  
    std::optional<pb::Solution> solution;

    struct TaskMeta {
      const key_type agent;
    };

    std::unordered_map<TaskID, TaskMeta> taskmeta;

    Graph(key_type k) : key {k} {}
  };

  // Webui actor.
  struct WebUI : ActorBase {

    const key_type key;

    WebUI(key_type k) : key {k} {}
  };

    bool _enqueue(const Graph&);
    bool _deploy(Graph&);
    bool _remove_agent(key_type);
    bool _remove_graph(key_type, bool);
    bool _remove_webui(key_type);

    size_t _num_agents() const;
    size_t _num_graphs() const;
    size_t _dequeue();

    void _on_resource(key_type, pb::Resource&);
    void _on_topology(key_type, pb::Topology&);
    void _on_taskinfo(key_type, pb::TaskInfo&);

    key_type _insert_graph(std::shared_ptr<Socket>&);
    key_type _insert_agent(std::shared_ptr<Socket>&);
    key_type _insert_webui(std::shared_ptr<Socket>&);
    
    ClusterInfo _cluster_info() const;

  public:

    Master();
    ~Master();
    
    std::future<void> on_resource(key_type, pb::Resource&&);
    std::future<void> on_topology(key_type, pb::Topology&&);
    std::future<void> on_taskinfo(key_type, pb::TaskInfo&&);

    std::future<bool> remove_agent(key_type);
    std::future<bool> remove_graph(key_type, bool);
    std::future<bool> remove_webui(key_type);

    std::future<key_type> insert_agent(std::shared_ptr<Socket>&&);
    std::future<key_type> insert_graph(std::shared_ptr<Socket>&&);
    std::future<key_type> insert_webui(std::shared_ptr<Socket>&&);

    void webui(std::shared_ptr<Socket>&&);
    HttpResponse make_response(const HttpRequest&);
    std::string agent_to_json(std::string_view);

    size_t num_graphs() const;
    size_t num_agents() const;
    size_t num_webuis() const;

    std::future<ClusterInfo> cluster_info();

  private:

    std::unordered_map<key_type, Agent> _agents;
    std::unordered_map<key_type, Graph> _graphs;
    std::unordered_map<key_type, WebUI> _webuis;

    std::queue<key_type> _queue;
};


};  // End of namespace dtc. --------------------------------------------------------------

#endif



