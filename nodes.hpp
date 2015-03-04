
#ifndef NODES_HPP
#define NODES_HPP


#include "headers.hpp"



template <size_t SizeStrArr>
class CBaseNode {
public:
	static const size_t chars_number = 256;
	static const unsigned char junior_ch = 0;
	
public:
	virtual bool IsLeaf () const = 0;
	virtual size_t GetEdgeLength (size_t current_end = -1) const = 0;
	virtual size_t GetBegin () const = 0;
	virtual void SetBegin (size_t i) = 0;
	
	//
	
	virtual size_t GetEnd () const {
		assert (false);
		return 0;
	}
	virtual CBaseNode* GetSuffLink () const  {
		assert (false);
		return nullptr;
	}
	virtual void SetSuffLink (CBaseNode* suff_link) {
		assert (false);
		return;
	}
	virtual CBaseNode* GetChild (char ch) const {
		assert (false);
		return nullptr;
	}
	virtual void SetChild (CBaseNode* child, char ch) {
		assert (false);
		return;
	}
	
	//
	
	size_t GetCharIndex (char ch) const {
		return static_cast <unsigned char> (ch) - junior_ch;
	}
	CBaseNode* GetParent () const {
		return m_parent;
	}
	void SetParent (CBaseNode* node) {
		m_parent = node;
		return;
	}
	void AddStringIndex (size_t index) {
		assert (index < SizeStrArr);
		m_str_ind_arr[index] = 1;
		return;
	}
	bool HasIndex (size_t index) const {
		assert (index < SizeStrArr);
		return m_str_ind_arr[index];
	}
	bool HasAllIndexes () const {
		unsigned counter = 0;
		std::for_each (
			&m_str_ind_arr[0],
			&m_str_ind_arr[0] + SizeStrArr,
			[&] (unsigned char *ptr)->void {counter += *ptr;}
		);
		return counter == SizeStrArr;
	}
	size_t GetPrimaryIndex () const {
		return m_primary_index;
	}
	
	//
	
	CBaseNode (CBaseNode * p, size_t primary_index):
		m_parent (p), m_primary_index(primary_index)
	{
		std::fill (&m_str_ind_arr[0], &m_str_ind_arr[0] + SizeStrArr, 0);
		return;
	}
	virtual ~ CBaseNode () {}

private:
	unsigned char m_str_ind_arr[SizeStrArr];
	size_t m_primary_index;
	CBaseNode* m_parent;
};

// ================================================================

template <size_t SizeStrArr>
class CFastLeafNode: public CBaseNode <SizeStrArr> {
public:
	virtual bool IsLeaf () const {
		return true;
	}
	virtual size_t GetEdgeLength (size_t current_end) const {
		assert (current_end != -1);
		return current_end - m_i;
	}
	virtual size_t GetBegin () const {
		return m_i;
	}
	virtual void SetBegin (size_t i) {
		m_i = i;
		return;
	}
	
	CFastLeafNode (size_t i, CBaseNode <SizeStrArr>* parent, size_t primary_index):
		m_i(i),
		CBaseNode <SizeStrArr> (parent, primary_index)
	{}
	
	~ CFastLeafNode () {}
	
private:
	// substring is [m_i, current_end)
	size_t m_i;
};

// ================================================================

template <size_t SizeStrArr>
class CFastInternalNode: public CBaseNode <SizeStrArr> {
public:
	typedef CBaseNode <SizeStrArr> BaseNode;

public:
	virtual bool IsLeaf () const {
		return false;
	}
	virtual size_t GetEdgeLength (size_t current_end) const {
		return m_j - m_i;
	}
	virtual size_t GetBegin () const {
		return m_i;
	}
	virtual void SetBegin (size_t i) {
		m_i = i;
		return;
	}
	virtual size_t GetEnd () const {
		return m_j;
	}
	virtual BaseNode* GetSuffLink () const  {
		return m_suff_link;
	}
	virtual void SetSuffLink (BaseNode* suff_link) {
		m_suff_link = suff_link;
		return;
	}
	virtual BaseNode* GetChild (char ch) const {
		return m_childs[BaseNode::GetCharIndex (ch)];
	}
	virtual void SetChild (BaseNode* child, char ch) {
		m_childs[BaseNode::GetCharIndex (ch)] = child;
		return;
	}
	
	
	CFastInternalNode (
		size_t i,
		size_t j,
		BaseNode* parent, 
		size_t primary_index
	):
		m_i(i),
		m_j(j),
		m_suff_link (nullptr),
		BaseNode(parent, primary_index)
	{
		assert (!parent->IsLeaf ());
		std::fill (&m_childs[0], &m_childs[0] + BaseNode::chars_number, nullptr);
		return;
	}
	
	~ CFastInternalNode ()
	{
		std::for_each (
			&m_childs[0],
			&m_childs[0] + BaseNode::chars_number,
			[] (BaseNode* ptr)->void {if (ptr) delete ptr;}
		);
		return;
	}
	
private:
	// substring is [m_i, m_j)
	size_t m_i;
	size_t m_j;
	BaseNode* m_suff_link;
	BaseNode* m_childs [BaseNode::chars_number];
};


#endif // NODES_HPP





