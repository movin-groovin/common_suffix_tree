
// g++ -std=c++11 algo.cpp -o cst -g
// g++ -std=c++11 algo.cpp -o cst -g -pg
// g++ -std=c++11 algo.cpp -o cst -DNDEBUG
// g++ -std=c++11 algo.cpp -o cst -DNDEBUG -DMEMORY_TEST

#include "headers.hpp"
#include "nodes.hpp"
#include <typeinfo>



namespace NMSAlgo {
	
	template <size_t SizeStrArr = 10>
	class CCommonSuffixTree {
	public:
		typedef CBaseNode <SizeStrArr> BaseNode;
		typedef State <BaseNode*> BaseState;
		typedef Ret <BaseNode*> BaseRet;
		
		struct PRINT_ENTRY {
			BaseNode* element;
			unsigned char cnt;
			std::string space;
		};
		
	private:
		BaseNode* m_root;
		std::string m_str_arr[SizeStrArr];
		//std::string m_str_terminals[SizeStrArr];
		std::string m_one_str;
		size_t m_one_str_index;
		
		
		bool IsRoot (const BaseNode* ptr_link) const {
			return (ptr_link->GetBegin () == 0) && (ptr_link->GetEnd () == 0);
		}
		
		virtual BaseNode* MakeLeafNode (size_t i, BaseNode* parent, size_t primary_index) {
			return new CFastLeafNode<SizeStrArr> (i, parent, primary_index);
		}
		
		virtual BaseNode* MakeInetrnalNode (size_t i, size_t j, BaseNode* parent, size_t primary_index) {
			return new CFastInternalNode<SizeStrArr> (i, j, parent, primary_index);
		}
		
		size_t GetRightEdgeLength (BaseNode *node, size_t end) const {
			if (node->IsLeaf() && node->GetPrimaryIndex() != m_one_str_index) {
				return m_str_arr[node->GetPrimaryIndex ()].length() - node->GetBegin();
			}
			else {
				return node->GetEdgeLength(end);
			}
		}
		
		void CopyStrIndexesLeafsToInternals ();
		
	public:
		CCommonSuffixTree ():
			m_root (MakeInetrnalNode (0, 0, nullptr, -1)),
			m_one_str_index (-1)
		{
			m_root->SetSuffLink (m_root);
			m_root->SetParent (m_root);
			
			return;
		}
		
		virtual ~ CCommonSuffixTree () {delete m_root;}
		// ----------------------------------------------------------
		void ConstructByUkkonenAlgo (
			const std::vector <std::string> & data,
			const std::vector <std::string> & str_terminals
		)
		{
			if (data.size() > SizeStrArr) {
				std::ostringstream ossCnv;
				ossCnv << SizeStrArr;
				throw std::logic_error (
					"Too many srtings for inserting, max = " + ossCnv.str()
				);
			}
			
			for (unsigned i = 0; i < data.size(); ++i) {
				m_one_str = m_str_arr[i] = data[i] + str_terminals[i];
				m_one_str_index = i;
				BaseState ret_inf = {m_root, 0, 0};
				for (size_t j = 0; j < m_one_str.length (); ++j) {
					ret_inf = AppendChar (ret_inf.node, ret_inf.ch, ret_inf.length, j);
				}
			}
			
			return;
		}
		// ----------------------------------------------------------
		BaseNode* InsertNode (BaseNode* parent, size_t i)
		{
			BaseNode* new_node (MakeLeafNode (i, parent, m_one_str_index));
			new_node->AddStringIndex (m_one_str_index);
			parent->SetChild (new_node, m_one_str[i]);
			
			return new_node;
		}
		// ----------------------------------------------------------
		BaseNode* SplitEdge (BaseNode* base, size_t len) {
			BaseNode* parent = base->GetParent ();
			BaseNode* new_node (MakeInetrnalNode (
					base->GetBegin (),
					base->GetBegin () + len,
					parent,
					m_one_str_index
				)
			);
			
			new_node->SetChild (base, m_one_str [base->GetBegin () + len]);
			parent->SetChild (new_node, m_one_str [base->GetBegin ()]);
			
			base->SetBegin (base->GetBegin () + len);
			base->SetParent (new_node);
			
			return new_node;
		}
		// ----------------------------------------------------------
		void PrintDfs () const {
			std::stack <PRINT_ENTRY> stack;
			std::string base_space ("  ");
			
			
			stack.push (PRINT_ENTRY {m_root, 0, ""});
			while (!stack.empty ())
			{
				BaseNode* proc_elem = stack.top().element;
				unsigned char proc_cnt = stack.top().cnt;
				
				for (int cnt = proc_cnt; cnt <= BaseNode::chars_number - 1; ++cnt)
				{
					BaseNode* child = proc_elem->GetChild (cnt);
					if (child) {
						PrintEdge (child, stack.top().space);
						if (!child->IsLeaf ()) {
							stack.top().cnt = cnt + 1;
							stack.push (PRINT_ENTRY {
									child,
									0,
									stack.top().space + base_space
								}
							);
							goto NEXT_LEVEL;
						}
						else {
							continue;
						}
					}
				}
				stack.pop();
				
				NEXT_LEVEL:;
			}
			
			
			return;
		}
		
		void PrintEdge (BaseNode* node, const std::string & prefix) const {
			const std::string &edge_str = m_str_arr[node->GetPrimaryIndex ()];
			
			std::cout << prefix << std::string().assign (
				edge_str,
				node->GetBegin(),
				node->GetEdgeLength(edge_str.length())
			) << '\n';
			
			return;
		}
		
		BaseState WalkDown (
			BaseNode* from,
			size_t len,
			char ch,
			size_t i
		) const
		{
			BaseState ret {from, len, ch};

			while (ret.length && GetRightEdgeLength (ret.node->GetChild (ret.ch), i + 1) < ret.length) {
				ret.node = ret.node->GetChild (ret.ch);
				ret.length -= GetRightEdgeLength (ret.node, i + 1);
				ret.ch = m_one_str[i - ret.length];
			}
			if (!ret.length) {
				ret.ch = 0;
			}
			if (ret.length && GetRightEdgeLength (ret.node->GetChild (ret.ch), i + 1) == ret.length) {
				ret.node = ret.node->GetChild (ret.ch);
				ret.length = 0;
				ret.ch = 0;
			}
			
			return ret;
		}
		
		BaseRet AddSuffix (
			BaseNode* node_walk_from,
			char ch,
			size_t len,
			size_t i
		);
		
		BaseState AppendChar (
			BaseNode* node,
			char ch,
			size_t length,
			size_t i
		);
	};
	// ====================================================================================
	template <size_t SizeStrArr>
	Ret <CBaseNode <SizeStrArr>*> CCommonSuffixTree <SizeStrArr>::AddSuffix (
		BaseNode* node_walk_from,
		char ch,
		size_t len,
		size_t i
	)
	{
		BaseState start = WalkDown (node_walk_from, len, ch, i);
		node_walk_from = start.node;
		len = start.length;
		ch = start.ch;
		
		
		if (!len) {
			assert (ch == 0);			
			if (node_walk_from->GetChild (m_one_str [i])) {
				return BaseRet {nullptr, nullptr};
			}
			else {		
				InsertNode (node_walk_from, i);
				return BaseRet {nullptr, node_walk_from};
			}
		}
		else {
			// 'len' is everything less than 
			// 'node_walk_from->GetChild (ch)->GetEdgeLength(i)'
			assert (node_walk_from->GetChild (ch)->GetEdgeLength (i + 1) > len);
			
			BaseNode* node_begin = node_walk_from->GetChild (ch);
			std::string edge_str = m_str_arr[node_begin->GetPrimaryIndex ()];
			if (edge_str[node_begin->GetBegin () + len] == m_one_str[i]) {
				return BaseRet {nullptr, nullptr};
			}
			else {
				BaseNode* new_node = SplitEdge (node_begin, len);
				InsertNode (new_node, i);
				return BaseRet {new_node, nullptr};
			}
		}
		
		
		// never reaches this place
		assert (1 != 1);
		return BaseRet {nullptr, nullptr};
	}
	// ====================================================================================
	template <size_t SizeStrArr>
	State <CBaseNode <SizeStrArr>*> CCommonSuffixTree<SizeStrArr>::AppendChar (
		BaseNode* node,
		char ch,
		size_t length,
		size_t i
	)
	{
		BaseRet ret;
		BaseNode* prev_node = nullptr;
		BaseNode* exist_node (node);
		
		
		while (true) {
			ret = AddSuffix (exist_node, ch, length, i);
			
			// First case: the suffix already exists at the tree
			if (!ret.new_node && !ret.internal_node) 
			{
				if (prev_node && prev_node->GetSuffLink() == NULL) {
					BaseState start = WalkDown (exist_node, length, ch, i);
					
					prev_node->SetSuffLink (start.node);
				}
				
				if (!ch) { // && !length
					assert (length == 0);
					
					if (GetRightEdgeLength(exist_node->GetChild(m_one_str[i]), i + 1) == 1) {
						return BaseState {exist_node->GetChild(m_one_str[i]), 0, 0};
					}
					return BaseState {exist_node, 1, m_one_str[i]};
				}
				// GetEdgeLength (i + 1) -> i + 1, not i, because we have
				// added implicitly for every leaf node new character
				else if (GetRightEdgeLength(exist_node->GetChild(m_one_str[i]), i + 1) == length + 1) {
					exist_node = exist_node->GetChild (ch);
					return BaseState {exist_node, 0, 0};
				}
				else {
					return BaseState {exist_node, length + 1, ch};
				}
			}
			// Second case: exists edge has been splited, the edge of 
			// 1 character length has been inserted with new node
			else if (ret.new_node)
			{
				if (prev_node)
					prev_node->SetSuffLink (ret.new_node);
				prev_node = ret.new_node;
				
				if (ret.new_node->GetParent () == m_root) {
					ret.new_node->SetSuffLink(m_root);
					
					if (ret.new_node->GetEdgeLength () == 1) {
						ch = 0;
						length = 0;
						exist_node = ret.new_node->GetParent (); // = m_root
					}
					else {
						--length;
						exist_node = ret.new_node->GetParent();
						ch = m_one_str[ret.new_node->GetBegin() + 1];
					}
				}
				else {
					length = ret.new_node->GetEdgeLength();
					ch = m_one_str[ret.new_node->GetBegin()];
					exist_node = ret.new_node->GetParent()->GetSuffLink();
				}
			}
			// Third case: at exist (internal of course) node has been inserted
			// an one character length edge with the node at it's end
			else
			{
				assert (ret.internal_node);
				
				length = 0;
				ch = 0;
				
				if (prev_node)
					prev_node->SetSuffLink (ret.internal_node);
				prev_node = ret.internal_node;
				
				if (ret.internal_node == m_root) // exist_node == ret.internal_node
					return BaseState {m_root, 0, 0};
				
				exist_node = ret.internal_node->GetSuffLink ();
			}
			//
		}
		
		
		assert (1 != 1);
		// never reachs this place
	}
} // namespace


std::string ReadFromStreamUntilEof (std::istream & in_s, char no_ch) {
	std::string buf, ret;
	
	while (std::getline (in_s, buf, '\n')) {
		if (buf.end () != std::find (buf.begin (), buf.end (), no_ch)) {
			throw std::runtime_error (
				"Incorrect symbol in input string: " + std::string () + no_ch
			);
		}
		ret += buf;
	}
	
	return ret;
}


int main (int argc, char **argv) {
	try {
		std::vector<std::string> test_data {"abc", "bcb", "abcd"};
		std::vector <std::string> terminals {"@", "#", "$"};
		NMSAlgo::CCommonSuffixTree <> cs_tree;
		

		cs_tree.ConstructByUkkonenAlgo (test_data, terminals);
#ifndef NDEBUG
		std::cout << "\nStrings:\n";
		for (auto &val : test_data)
			std::cout << "  " << val << "\n";
		std::cout << "\n";
		std::cout << "Suffix tree: \n";
		cs_tree.PrintDfs ();
#endif
#ifdef MEMORY_TEST
		char ch;
		std::cout << "Press enter to continue: ";
		ch = getchar ();
#endif
		return 0;
	} catch (std::exception & Exc) {
		std::cout << Exc.what() << std::endl;
		
		return 0;
	}
}







