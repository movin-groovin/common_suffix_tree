
// g++ -std=c++11 ukkonen.cpp -o ukk_algo -g -pg
// g++ -std=c++11 ukkonen.cpp -o ukk_algo -DNDEBUG
// g++ -std=c++11 ukkonen.cpp -o ukk_algo -DNDEBUG -DMEMORY_TEST

// http://linuxtv.org/downloads/v4l-dvb-apis/capture-example.html
// http://www.linuxconsulting.ro/picturegrabber/
// http://habrahabr.ru/post/212531/
// http://docs.opencv.org/trunk/doc/py_tutorials/py_gui/py_video_display/py_video_display.html

#include "headers.hpp"
#include "nodes.hpp"



namespace NMSAlgo {
	
	template <typename SizeStrArr>
	class CCommonSuffixTree {
	public:
		typedef CBaseNode <SizeStrArr> BaseNode;
		
	private:
		BaseNode* m_root;
		std::string m_str_arr[SizeStrArr];
		
		bool IsRoot (const BaseNode* ptr_link) const {
			return (ptr_link->GetBegin () == 0) && (ptr_link->GetEnd () == 0);
		}
		
		virtual BaseNode* MakeLeafNode (size_t i, BaseNode* parent) {
			return new CFastLeafNode<SizeStrArr> (i, parent);
		}
		
		virtual BaseNode* MakeInetrnalNode (size_t i, size_t j, BaseNode* parent) {
			return new CFastInternalNode<SizeStrArr> (i, j, parent);
		}
		
	public:
		CCommonSuffixTree ():
			m_root (MakeInetrnalNode (0, 0, nullptr))
		{
			m_root->SetSuffLink (m_root);
			m_root->SetParent (m_root);
			
			return;
		}
		
		virtual ~ CCommonSuffixTree () {delete m_root;}
		
		
		
		
		
		
		void ConstructByUkkonenAlgo (const std::string & str) {
			State ret_inf = {m_root, 0, 0};
			
			m_str = str;
			
			for (size_t i = 0; i < m_str.length (); ++i) {
				ret_inf = AppendChar (ret_inf.node, ret_inf.ch, ret_inf.length, i);
			}
			
			return;
		}
		
		CNode* InsertNode (CNode* parent, size_t i)
		{
			CNode* new_node (new CNode (i, -1, parent));
			parent->SetChild (new_node, m_str[i]);
			
			return new_node;
		}
		
		CNode* SplitEdge (CNode* base, size_t len) {
			CNode* parent = base->GetParent ();
			CNode* new_node {new CNode (
					base->GetBegin (),
					base->GetBegin () + len,
					parent
				)
			};
			
			new_node->SetChild (base, m_str [base->GetBegin () + len]);
			parent->SetChild (new_node, m_str[base->GetBegin ()]);
			
			base->SetBegin (base->GetBegin () + len);
			base->SetParent (new_node);
			
			return new_node;
		}
		
		struct PRINT_ENTRY {
			CNode* element;
			unsigned char cnt;
			std::string space;
		};

		void PrintDfs () const {
			std::stack <PRINT_ENTRY> stack;
			std::string base_space ("  ");
			
			
			stack.push (PRINT_ENTRY {m_root, 0, ""});
			while (!stack.empty ())
			{
				CNode* proc_elem = stack.top().element;
				unsigned char proc_cnt = stack.top().cnt;
				
				for (int cnt = proc_cnt; cnt <= CNode::chars_number - 1; ++cnt)
				{
					CNode* child = proc_elem->GetChild (cnt);
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
		
		void PrintEdge (CNode* node, const std::string & prefix) const {
			std::cout << prefix << std::string().assign (
				m_str,
				node->GetBegin(),
				node->GetEdgeLength(m_str.length())
			) << '\n';
			
			return;
		}
		
		State WalkDown (
			CNode* from,
			size_t len,
			char ch,
			size_t i
		) const
		{
			State ret {from, len, ch};
			
			while (ret.length && ret.node->GetChild (ret.ch)->GetEdgeLength (i + 1) < ret.length) {
				ret.node = ret.node->GetChild (ret.ch);
				ret.length -= ret.node->GetEdgeLength (i + 1);
				ret.ch = m_str[i - ret.length];
			}
			if (!ret.length) {
				ret.ch = 0;
			}
			if (ret.length && ret.node->GetChild (ret.ch)->GetEdgeLength (i + 1) == ret.length) {
				ret.node = ret.node->GetChild (ret.ch);
				ret.length = 0;
				ret.ch = 0;
			}
			
			return ret;
		}
		
		Ret AddSuffix (
			CNode* node_walk_from,
			char ch,
			size_t len,
			size_t i
		);
		
		State AppendChar (
			CNode* node,
			char ch,
			size_t length,
			size_t i
		);
	};
	// ====================================================================================
	Ret CSuffixTree::AddSuffix (
		CNode* node_walk_from,
		char ch,
		size_t len,
		size_t i
	)
	{
		State start = WalkDown (node_walk_from, len, ch, i);
		node_walk_from = start.node;
		len = start.length;
		ch = start.ch;
		
		
		if (!len) {
			assert (ch == 0);			
			if (node_walk_from->GetChild (m_str [i])) {
				return Ret {nullptr, nullptr};
			}
			else {		
				InsertNode (node_walk_from, i);
				return Ret {nullptr, node_walk_from};
			}
		}
		else {
			// 'len' is everything less than 
			// 'node_walk_from->GetChild (ch)->GetEdgeLength(i)'
			assert (node_walk_from->GetChild (ch)->GetEdgeLength (i + 1) > len);
			
			CNode* node_begin = node_walk_from->GetChild (ch);
			if (m_str[node_begin->GetBegin () + len] == m_str[i]) {
				return Ret {nullptr, nullptr};
			}
			else {
				CNode* new_node = SplitEdge (node_begin, len);
				InsertNode (new_node, i);
				return Ret {new_node, nullptr};
			}
		}
		
		
		// never reaches this place
		assert (1 != 1);
		return Ret {nullptr, nullptr};
	}
	// ====================================================================================
	State CSuffixTree::AppendChar (
		CNode* node,
		char ch,
		size_t length,
		size_t i
	)
	{
		Ret ret;
		CNode* prev_node = nullptr;
		CNode* exist_node (node);
		
		
		while (true) {
			ret = AddSuffix (exist_node, ch, length, i);
			
			// First case: the suffix already exists at the tree
			if (!ret.new_node && !ret.internal_node) 
			{
				if (prev_node && prev_node->GetSuffLink() == NULL) {
					State start = WalkDown (exist_node, length, ch, i);
					
					prev_node->SetSuffLink (start.node);
				}
				
				if (!ch) { // && !length
					assert (length == 0);
					
					if (exist_node->GetChild(m_str[i])->GetEdgeLength(i + 1) == 1) {
						return State {exist_node->GetChild(m_str[i]), 0, 0};
					}
					return State {exist_node, 1, m_str[i]};
				}
				// GetEdgeLength (i + 1) -> i + 1, not i, because we have
				// added implicitly for every leaf node new character
				else if (exist_node->GetChild (ch)->GetEdgeLength (i + 1) == length + 1) {
					exist_node = exist_node->GetChild (ch);
					return State {exist_node, 0, 0};
				}
				else {
					return State {exist_node, length + 1, ch};
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
						ch = m_str[ret.new_node->GetBegin() + 1];
					}
				}
				else {
					length = ret.new_node->GetEdgeLength();
					ch = m_str[ret.new_node->GetBegin()];
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
					return State {m_root, 0, 0};
				
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
		const char fin_ch = '$';
		std::string test_str = "ababc";
		NMSUkkonenAlgo::CSuffixTree suff_tree;
		
		if (argc > 1) {
			std::ifstream ifs (argv[1]);
			if (!ifs) {
				std::cout << "Can't open the file: " << argv[1] << "\n";
				return 10002;
			}
			std::getline (ifs, test_str);
		} else {
			test_str = ReadFromStreamUntilEof (std::cin, fin_ch);
		}
		
		
		test_str += fin_ch;
		
		suff_tree.ConstructByUkkonenAlgo (test_str);
#ifndef NDEBUG
		std::cout << "\nString: " << test_str << "\n\n";
		std::cout << "Suffix tree: \n";
		suff_tree.PrintDfs ();
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







