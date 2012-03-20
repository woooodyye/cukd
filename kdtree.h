// Copyright (c) 2012, Thomas Schutzmeier
// FreeBSD License
// See https://github.com/unvirtual/cukd/blob/master/LICENSE

#ifndef CUKD_KDTREE_H
#define CUKD_KDTREE_H

#include <boost/shared_ptr.hpp>
#include <thrust/functional.h>
#include "utils/utils.h"
#include "utils/primitives.h"
#include "detail/node_chunk_array.h"
#include "detail/kdtree_node_array.h"
#include "detail/small_node_array.h"

namespace cukd { 

const unsigned int leaf_mask = 0x80000000;
const unsigned int n_element_mask = 0x7fffffff;
const unsigned int right_node_mask = 0x7ffffff;
const unsigned int left_empty_mask = 0x40000000;
const unsigned int right_empty_mask = 0x20000000;
const unsigned int split_axis_mask = 0x18000000;
const unsigned int split_axis_shift = 27;

// temporary, will be removed
struct UAABB {
    UFloat4 minimum;
    UFloat4 maximum;
};

// temporary, will be removed
struct KDTreeHost {
    std::vector<int> split_axis;
    std::vector<float> split_position;
    std::vector<int> left_nodes;
    std::vector<int> right_nodes;
    std::vector<UAABB> small_node_aabbs;
    std::vector<int> element_idx;
    std::vector<int> node_element_first_idx;
    std::vector<int> element_size;
    std::vector<int> leaf_index;
    int elements;
    int large_nodes;
};

/**********************************************************************************
 *
 * KDTree
 *
 **********************************************************************************/

using cutl::DevVector;
using cutl::AABB;
using cutl::TriangleArray;

class KDTree {
    public:
        KDTree(AABB & tree_aabb, TriangleArray & tris, int small_threshold);
        void create();
        void preorder();
        int max_depth() { return max_depth_; };
        // TODO: remove or improve substantially
        void print();
        void print_preorder();

        // temporary, will be removed
        KDTreeHost to_host();
        std::vector<AABB> small_aabbs();

    private:
        // no copies, no assignments
        KDTree(const KDTree &);
        KDTree& operator=(const KDTree &);

        void large_nodes_process(NodeChunkArray & active, NodeChunkArray & next);
        int small_nodes_process(SmallNodeArray & active, SmallNodeArray & next,
                                SplitCandidateArray & sca, int init_leaf_nodes);

        // Splits the active_nca into child nodes in next_nca, thereby
        // updating the tree node association list (active_indices).
        int split_into_children(NodeChunkArray & active_nca,
                                NodeChunkArray & next_nca,
                                DevVector<int> & active_indices);

        // removes small nodes (<= _small_threshold elements) from nca and adds them
        // to small_nca
        void remove_small_nodes(NodeChunkArray & nca);

    private:
        AABB root_aabb;
        TriangleArray triangles;
        int _small_threshold;
        int max_depth_;

        NodeChunkArray active_nca;
        NodeChunkArray next_nca;
        SmallNodeArray small_nca;
        KDTreeNodeArray tree_nca;

        DevVector<unsigned int> preorder_tree;


        // helper vectors, avoid mallocs
        // TODO: weed out
        DevVector<int> n_empty_nodes;
        DevVector<int> cut_dirs;
        DevVector<int> active_indices;

        DevVector<int> chunk_sums;
        DevVector<int> node_sums;
        DevVector<int> first_node_idx;
        DevVector<int> out_keys;
        DevVector<int> temp;

        DevVector<int> child_diff;
        DevVector<int> tags;
        DevVector<int> small_tag;
        DevVector<int> small_elem_tags;
};


}  // namespace cukd

#endif // CUKD_KDTREE_H