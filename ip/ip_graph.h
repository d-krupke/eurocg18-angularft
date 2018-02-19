//
// Created by Dominik Krupke, http://krupke.cc on 12/19/17.
//

#ifndef CODE_IP_GRAPH_H
#define CODE_IP_GRAPH_H

#include <vector>
#include <limits>

#include "../instance/point.h"
#include "./ip_point.h"
#include "../instance/angles.h"
#include "boost_graph.h"
#include "../instance/problem_instance.h"

namespace angularfreezetag {

    class IpGraph {
    public:

        size_t GetNumVertices() const { return boost::num_vertices(graph_); }

        size_t GetNumEdges() const { return boost::num_edges(graph_); }

        auto Vertices() const {
            return boost::make_iterator_range(boost::vertices(graph_));
        }

        auto Edges() const {
            return boost::make_iterator_range(boost::edges(graph_));
        }

        double CalculateRotationCost(Vertex v, Vertex w)
        const {
            const auto &vertex_name_map = boost::get(boost::vertex_name, graph_);
            auto h1 = boost::get(vertex_name_map, v).GetHeading();
            auto h2 = boost::get(vertex_name_map, w).GetHeading();
            return CalculateTurnAngle(h1, h2);

        }

        void AddEdgesToAllOtherVerticesOfPoint(Vertex v, const IpPoint &p) {
            for (const auto &np: p.GetNeighborHeadingVertices()) {
                const auto &n = np.second;
                if (v == n) continue;

                double cost = CalculateRotationCost(v, n);
                boost::add_edge(v, n, cost, graph_);
            }
            //TODO Check if we really don't need the part below. We don't want incoming edges for start vertices.
            /**
            for (const auto &np: p.GetStartHeadingVertices()) {
              const auto &n = np.second;
              if (v == n) continue;

              double cost = CalculateRotationCost(v, n);
              boost::add_edge(v, n, cost, graph_);
            }**/
        }

        void AddTurnEdges(const IpPoint &p) {
            for (const auto &vp: p.GetNeighborHeadingVertices()) {
                const auto &v = vp.second;
                AddEdgesToAllOtherVerticesOfPoint(v, p);
            }
            for (const auto &vp: p.GetStartHeadingVertices()) {
                const auto &v = vp.second;
                AddEdgesToAllOtherVerticesOfPoint(v, p);
            }
        }

        void AddStartVertex(const PointWithOrientation &p) {//Start Vertex
            VertexData vd{p.p, p.o};
            auto v = boost::add_vertex(graph_);
            boost::put(get(boost::vertex_name, graph_), v, vd);
            IpPoint &ipp = points_[p.p];
            ipp.AddStartHeading(p.o, v);
        }

        void AddNeighborHeadingVertices(const PointWithOrientation &p) {
            if (points_.count(p.p) > 0) return; //Already added
            points_[p.p] = {p.p};
            IpPoint &ipp = points_[p.p];
            for (auto &p_ip: points_) {
                const auto &vertex_name_map = boost::get(boost::vertex_name, graph_);
                auto &pp = p_ip.second;
                if (pp == ipp) continue;
                //if (pp.GetPoint() == start_point_) continue;

                if (p.p != start_point_) {
                    auto v1 = boost::add_vertex(graph_);
                    boost::put(vertex_name_map, v1, VertexData{pp.GetPoint(), p.p});
                    pp.AddHeading(p.p, v1);
                }

                if (pp.GetPoint() != start_point_) {
                    auto v2 = boost::add_vertex(graph_);
                    boost::put(vertex_name_map, v2, VertexData{p.p, pp.GetPoint()});
                    ipp.AddHeading(pp.GetPoint(), v2);
                }
            }
        }

        double GetEdgeCost(Edge e) {
            const auto &edge_cost_map = boost::get(boost::edge_weight, graph_);
            assert(boost::get(edge_cost_map, e) >= 0);
            return boost::get(edge_cost_map, e);
        }

        VertexData GetVertexData(Vertex v) {
            const auto &vertex_data_map = boost::get(boost::vertex_name, graph_);
            return boost::get(vertex_data_map, v);
        }

        explicit
        IpGraph(const std::vector<PointWithOrientation> &points, const Point &start_point) :
                graph_{}, points_{}, start_point_{start_point} {
            for (const auto &p: points) {
                AddNeighborHeadingVertices(p);
                AddStartVertex(p);
            }

            for (const auto &p: points_) {
                AddTurnEdges(p.second);
            }
        }

        IpGraph() : graph_{}, points_{}, start_point_{} {}

        const auto &GetPoints() const { return points_; }

        Vertex GetEdgeTarget(Edge e) const { return boost::target(e, graph_); }

        Vertex GetEdgeSource(Edge e) const { return boost::source(e, graph_); }

    private:
        BoostGraph graph_;
        std::unordered_map<Point, IpPoint, PointHasher> points_;
        Point start_point_;
    };
}
#endif //CODE_IP_GRAPH_H
