/*
 * Name:        svbitree.c
 * Description: Binary trees.
 * Author:      cosh.cage#hotmail.com
 * File ID:     0809171737G0503231616L00542
 *
 * The following text is copied from the source code of SQLite and padded
 * with a little bit addition to fit the goals for StoneValley project:
 *
 * The author disclaims copyright to this source code. In place of
 * a legal notice, here is a blessing:
 *   May you do good and not evil.
 *   May you find forgiveness for yourself and forgive others.
 *   May you share freely, never taking more than you give.
 *   Hope you never need to push yourself or other people too hard.
 */

#include <stdlib.h> /* Using function malloc, free. */
#include "svtree.h"
#include "svqueue.h"

/* A structure for binary tree copy. */
typedef struct _st_TreeCopy {
	void *    pnroot; /* Root of a new tree.   */
	P_QUEUE_L pquel;  /* Children nodes queue. */
	size_t    size;   /* Size of node data.    */
} _TreeCopy, * _P_TreeCopy;

/* File-level function declarations here. */
extern int _strCBFDeleteNode       (void * pitem, size_t param);
extern int _strCBFNodesCounter     (void * pitem, size_t param);
extern int _strCBFCompareNodeDataD (void * pitem, size_t param);
int        _treCBFParentRetriver   (void * pitem, size_t param);
int        _treCBFNodeLocator      (void * pitem, size_t param);
int        _treCBFCopyTreeNodeB    (void * pitem, size_t param);

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _treCBFParentRetriver
 * Description:   This function is used to find parent in binary tree.
 * Parameters:
 *      pitem Pointer to each node in the tree.
 *      param Pointer to a child node whose parent is about to search.
 * Return value:  If parent node had been found, function would return a CBF_TERMINATE.
 *                If parent node could not find in a tree, function would return a CBF_CONTINUE.
 */
int _treCBFParentRetriver(void * pitem, size_t param)
{
	/* The current node is the parent node of a searching target. */
	if (*(P_TNODE_BY *)param == ((P_TNODE_BY)pitem)->ppnode[LEFT])
	{
		*(P_TNODE_BY *)param = (P_TNODE_BY)pitem;
		return CBF_TERMINATE;
	}
	if (*(P_TNODE_BY *)param == ((P_TNODE_BY)pitem)->ppnode[RIGHT])
	{
		*(P_TNODE_BY *)param = (P_TNODE_BY)pitem;
		return CBF_TERMINATE;
	}
	return CBF_CONTINUE; /* Continue searching. */
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _treCBFNodeLocator
 * Description:   This function is used to find a specific node in binary tree.
 * Parameters:
 *      pitem Pointer to each node in a tree.
 *      param Pointer to a FindingInfo structure.
 * Return value:  If the specific node had been found, function would return a CBF_TERMINATE.
 *                If the specific node could not find in a tree, function would return a CBF_CONTINUE.
 */
int _treCBFNodeLocator(void * pitem, size_t param)
{
	P_FindingInfo pfi = (P_FindingInfo)param;
	if (pfi->pitem == pitem)
	{
		pfi->size = TRUE;
		return CBF_TERMINATE;
	}
	return CBF_CONTINUE;
}

/* Attention:     This Is An Internal Function. No Interface for Library Users.
 * Function name: _treCBFCopyTreeNodeB
 * Description:   This function is used to copy nodes in binary tree.
 * Parameters:
 *      pitem Pointer to each node in the tree.
 *      param Pointer to a _TreeCopy structure.
 * Return value:  If any error occurred while copying, function would return a CBF_TERMINATE.
 *                If there were no error produced while copying, function would return a CBF_CONTINUE.
 */
int _treCBFCopyTreeNodeB(void * pitem, size_t param)
{
	size_t tptr;
	_P_TreeCopy ptc = (_P_TreeCopy)param;
	P_TNODE_BY pcur = (P_TNODE_BY)pitem;
	P_TNODE_BY pnew = strCreateNodeD(pcur->pdata, ptc->size);
	if (NULL == pnew)
	{	/* Allocation failure. */
		treFreeBY((P_BYTREE)&ptc->pnroot);
		ptc->pnroot = NULL;
		return CBF_TERMINATE;
	}
	/* Set new root. */
	if (NULL == ptc->pnroot)
		ptc->pnroot = pnew;
	if (! queIsEmptyL(ptc->pquel))
	{	/* Solve every parents' child nodes for the previous level. */
		P_TNODE_BY * ppnode;
		queRemoveL(&ppnode, sizeof(P_TNODE_BY), ptc->pquel);
		*ppnode = pnew;
	}
	/* Record the needed child for the next tree level. */
	if (NULL != pcur->ppnode[LEFT])
	{
		tptr = (size_t)&(pnew->ppnode[LEFT]);
		queInsertL(ptc->pquel, &tptr, sizeof(P_TNODE_BY));
	}
	if (NULL != pcur->ppnode[RIGHT])
	{
		tptr = (size_t)&(pnew->ppnode[RIGHT]);
		queInsertL(ptc->pquel, &tptr, sizeof(P_TNODE_BY));
	}
	return CBF_CONTINUE;
}

/* Function name: treTraverseBYPre
 * Description:   Traverse a binary tree by pre-order.
 * Parameters:
 *      pnode Pointer to the node that you want to start traversal in a tree.
 *     cbftvs Pointer to a callback function.
 *      param Parameter which can be transfer into the callback function.
 * Return value:  The same value as callback function returns.
 */
int treTraverseBYPre(P_TNODE_BY pnode, CBF_TRAVERSE cbftvs, size_t param)
{
	int r1 = CBF_CONTINUE, r2 = CBF_CONTINUE;
	if (NULL == pnode)
		return CBF_CONTINUE;
	if (CBF_CONTINUE != cbftvs(pnode, param))
		return CBF_TERMINATE;
	r1 = treTraverseBYPre(pnode->ppnode[LEFT],  cbftvs, param);
	r2 = treTraverseBYPre(pnode->ppnode[RIGHT], cbftvs, param);
	return CBF_CONTINUE == r1 ? r2 : r1;
}

/* Function name: treTraverseBYIn
 * Description:   Traverse a binary tree by in-order.
 * Parameters:
 *      pnode Pointer to the node that you want to start traversal in a tree.
 *     cbftvs Pointer to a callback function.
 *      param Parameter which can be transfer into the callback function.
 * Return value:  The same value as callback function returns.
 */
int treTraverseBYIn(P_TNODE_BY pnode, CBF_TRAVERSE cbftvs, size_t param)
{
	int r1 = CBF_CONTINUE, r2 = CBF_CONTINUE;
	if (NULL == pnode)
		return CBF_CONTINUE;
	r1 = treTraverseBYIn(pnode->ppnode[LEFT],  cbftvs, param);
	if (CBF_CONTINUE != cbftvs(pnode, param))
		return CBF_TERMINATE;
	r2 = treTraverseBYIn(pnode->ppnode[RIGHT], cbftvs, param);
	return CBF_CONTINUE == r1 ? r2 : r1;
}

/* Function name: treTraverseBYPost
 * Description:   Traverse a binary tree by post-order.
 * Parameters:
 *      pnode Pointer to the node that you want to start traversal in a tree.
 *     cbftvs Pointer to a callback function.
 *      param Parameter which can be transfer into the callback function.
 * Return value:  The same value as callback function returns.
 */
int treTraverseBYPost(P_TNODE_BY pnode, CBF_TRAVERSE cbftvs, size_t param)
{
	int r1 = CBF_CONTINUE, r2 = CBF_CONTINUE;
	if (NULL == pnode)
		return CBF_CONTINUE;
	r1 = treTraverseBYPost(pnode->ppnode[LEFT],  cbftvs, param);
	r2 = treTraverseBYPost(pnode->ppnode[RIGHT], cbftvs, param);
	if (CBF_CONTINUE != cbftvs(pnode, param))
		return CBF_TERMINATE;
	return CBF_CONTINUE == r1 ? r2 : r1;
}

/* Function name: treTraverseBYLevel
 * Description:   Traverse a binary tree by level-order.
 * Parameters:
 *      pnode Pointer to the node that you want to start traversal in a tree.
 *     cbftvs Pointer to a callback function.
 *      param Parameter which can be transfer into the callback function.
 * Return value:  The same value as callback function returns.
 */
int treTraverseBYLevel(P_TNODE_BY pnode, CBF_TRAVERSE cbftvs, size_t param)
{
	int r1 = CBF_CONTINUE, r2 = CBF_CONTINUE;
	QUEUE_L q;
	queInitL(&q);
	queInsertL(&q, &pnode, sizeof(P_TNODE_BY));
	while (! queIsEmptyL(&q))
	{
		queRemoveL(&pnode, sizeof(P_TNODE_BY), &q);
		if (CBF_CONTINUE != cbftvs(pnode, param))
			return CBF_TERMINATE;
		if (NULL != pnode->ppnode[LEFT])
			r1 = queInsertL(&q, &(pnode->ppnode[LEFT]),  sizeof(P_TNODE_BY));
		if (NULL != pnode->ppnode[RIGHT])
			r2 = queInsertL(&q, &(pnode->ppnode[RIGHT]), sizeof(P_TNODE_BY));
	}
	/* Do NOT forget to clean the queue. */
	queFreeL(&q);
	return CBF_CONTINUE == r1 ? r2 : r1;
}

/* Function name: treInitBY_O
 * Description:   Initialize a binary tree.
 * Parameter:
 *     ptreb Pointer to the binary tree that you want to initialize.
 * Return value:  N/A.
 * Caution:       Address of ptreb Must Be Allocated first.
 * Tip:           A macro version of this function named treInitB_M is available.
 */
void treInitBY_O(P_BYTREE ptreb)
{
	*ptreb = NULL;
}

/* Function name: treFreeBY
 * Description:   Deallocate a binary tree of which is allocated by function treInitBY_O.
 * Parameter:
 *     ptreb Pointer to the binary tree that you want to deallocate.
 * Return value:  N/A.
 * Caution:       Address of ptreb Must Be Allocated first.
 */
void treFreeBY(P_BYTREE ptreb)
{
	/* A post-order traversal is needed here.
	 * Because we have to free nodes from the crown to the root.
	 */
	treTraverseBYPost(*ptreb, _strCBFDeleteNode, ENT_DOUBLE);
	*ptreb = NULL;
}

/* Function name: treCreateBY
 * Description:   Allocate a new binary tree dynamically.
 * Parameter:     N/A.
 * Return value:  Pointer to the new allocated node.
 */
P_BYTREE treCreateBY(void)
{
	P_BYTREE ptreb = (P_BYTREE) malloc(sizeof(BYTREE));
	if (NULL == ptreb)
		return NULL;
	treInitBY(ptreb);
	return ptreb;
}

/* Function name: treDeleteBY_O
 * Description:   Delete a binary tree of which is allocated by function treCreateBY.
 * Parameter:
 *     ptreb Pointer to the binary tree that you want to allocate.
 * Return value:  N/A.
 * Caution:       Address of ptreb Must Be Allocated first.
 * Tip:           A macro version of this function named treDeleteB_M is available.
 */
void treDeleteBY_O(P_BYTREE ptreb)
{
	treFreeBY(ptreb);
	free(ptreb);
}

/* Function name: treInsertLeftBY
 * Description:   Insert a node as the left child of pnode.
 * Parameters:
 *      pnode Pointer to the parent node.
 *      pitem Pointer to the data you want to insert into a tree.
 *       size Size of data.
 * Return value:  If insertion succeeded, function would return a pointer of the new inserted node,
 *                otherwise function would return a NULL.
 * Caution:       If pnode equaled NULL, function would create a new node and return its pointer.
 */
P_TNODE_BY treInsertLeftBY(P_TNODE_BY pnode, const void * pitem, size_t size)
{
	if (NULL == pnode) /* If and only if pnode is a root node. */
		return (pnode = strCreateNodeD(pitem, size));
	if (NULL != pnode->ppnode[LEFT]) /* Left node already exists. */
		return NULL;
	return (pnode->ppnode[LEFT] = strCreateNodeD(pitem, size));
}

/* Function name: treInsertRightBY
 * Description:   Insert a node as the right child of pnode.
 * Parameters:
 *      pnode Pointer to the parent node.
 *      pitem Pointer to the data you want to insert into the tree.
 *       size Size of data.
 * Return value:  If insertion succeeded, function would return a pointer of the new inserted node,
 *                otherwise function would return a NULL.
 * Caution:       If pnode equaled NULL, function would create a new node and return a pointer to the address of element.
 */
P_TNODE_BY treInsertRightBY(P_TNODE_BY pnode, const void * pitem, size_t size)
{
	if (NULL == pnode) /* If and only if pnode is a root node. */
		return (pnode = strCreateNodeD(pitem, size));
	if (NULL != pnode->ppnode[RIGHT]) /* Right node already exists. */
		return NULL;
	return (pnode->ppnode[RIGHT] = strCreateNodeD(pitem, size));
}

/* Function name: treRemoveLeftBY
 * Description:   Remove the left node of pnode.
 * Parameter:
 *     pnode Pointer to the parent node which contains the left child node that you want to remove.
 * Return value:  N/A.
 * Caution:       The whole sub tree that left child node of pnode holds will be deallocated!
 */
void treRemoveLeftBY(P_TNODE_BY pnode)
{
	treTraverseBYPost(pnode->ppnode[LEFT], _strCBFDeleteNode, ENT_DOUBLE);
	pnode->ppnode[LEFT] = NULL;
}

/* Function name: treRemoveRightBY
 * Description:   Remove the right node of pnode.
 * Parameter:
 *     pnode Pointer to the parent node which contains the right child node that you want to remove.
 * Return value:  N/A.
 * Caution:       The whole sub tree that right child node of pnode holds will be deallocated!
 */
void treRemoveRightBY(P_TNODE_BY pnode)
{
	treTraverseBYPost(pnode->ppnode[RIGHT], _strCBFDeleteNode, ENT_DOUBLE);
	pnode->ppnode[RIGHT] = NULL;
}

/* Function name: treArityBY
 * Description:   Returns how many nodes in a binary tree.
 * Parameter:
 *     pnode Pointer to a node of a binary tree. It is usually the root node.
 * Return value:  Nodes number in a binary tree.
 */
size_t treArityBY(P_TNODE_BY pnode)
{
	size_t s = 0;
	/* No matter what order we use here, it is not a question. */
	treTraverseBYIn(pnode, _strCBFNodesCounter, (size_t)&s);
	return s;
}

/* Function name: treHeightBY
 * Description:   Test the height of a node in a binary tree.
 * Parameter:
 *     pnode Pointer to a node of a binary tree.
 * Return value:  The height of a node.
 * Tip:           If pnode were the root node of a binary tree,
 *                the height of that tree would return.
 */
size_t treHeightBY(P_TNODE_BY pnode)
{
	size_t dl = 0, dr = 0;
	if (NULL == pnode)
		return 0;
	dl = treHeightBY(pnode->ppnode[LEFT]);
	dr = treHeightBY(pnode->ppnode[RIGHT]);
	if (dl > dr)
		return dl + 1;
	else
		return dr + 1;
}

/* Function name: treGetParentNodeBY
 * Description:   Locate parent node of pchild in a binary tree.
 * Parameters:
 *      proot Pointer to the root node of a binary tree.
 *     pchild Pointer to the child node (who lost its parent. Kidding kidding kidding...).
 * Return value:  Pointer to parent node of pchild.
 */
P_TNODE_BY treGetParentNodeBY(P_TNODE_BY proot, P_TNODE_BY pchild)
{
	/* A pre-order traversal is needed here.
	 * Because a parent node is near by the root side.
	 */
	if (proot == pchild)
		return NULL; /* Parent of the root node is NULL. */
	treTraverseBYPre(proot, _treCBFParentRetriver, (size_t)&pchild);
	return pchild;
}

/* Function name: treSearchDataBY
 * Description:   Search for the specific data in a binary tree.
 * Parameters:
 *      pnode Pointer to the first node that you want to start your searching procedure in a binary tree.
 *      pitem Pointer to the data you want to search.
 *       size Size of that data.
 *         tm Method of traversal. This parameter can be any value in enumeration TvsMtd.
 * Return value:  Pointer to a node in the binary tree that contains the same data as pitem referred.
 *                If the specific data could not find in the tree, function would return a NULL.
 */
P_TNODE_BY treSearchDataBY(P_TNODE_BY pnode, const void * pitem, size_t size, TvsMtd tm)
{
	FindingInfo fi;
	fi.result = NULL;
	fi.pitem  = pitem;
	fi.size   = size;
	switch (tm)
	{
	case ETM_PREORDER:   treTraverseBYPre  (pnode, _strCBFCompareNodeDataD, (size_t)&fi); break;
	case ETM_INORDER:    treTraverseBYIn   (pnode, _strCBFCompareNodeDataD, (size_t)&fi); break;
	case ETM_POSTORDER:  treTraverseBYPost (pnode, _strCBFCompareNodeDataD, (size_t)&fi); break;
	case ETM_LEVELORDER: treTraverseBYLevel(pnode, _strCBFCompareNodeDataD, (size_t)&fi); break;
	}
	return (P_TNODE_BY)fi.result;
}

/* Function name: treDescendantBY
 * Description:   Test whether pnode is a descendant of a tree.
 *                And the root node of this tree is proot.
 * Parameters:
 *      proot Pointer to a root node of the tree you want to test.
 *      pnode Pointer to a node.
 * Return value:  If pnode were a descendant of a tree, function would return a TRUE,
 *                otherwise function would return a FALSE.
 */
BOOL treDescendantBY(P_TNODE_BY proot, P_TNODE_BY pnode)
{
	FindingInfo fi;
	if (proot == pnode || NULL == proot || NULL == pnode)
		return FALSE;
	fi.pitem = pnode;
	fi.size  = FALSE;
	treTraverseBYPre(proot, _treCBFNodeLocator, (size_t)&fi);
	return (BOOL)(fi.size ? TRUE: FALSE);
}

/* Function name: treMergeNodesBY
 * Description:   Merge two nodes into a binary tree.
 * Parameters:
 *      proot Pointer to the root node.
 *            If proot equaled value NULL, function would create a new node as a root node.
 *      pitem Pointer to the data of proot.
 *       size Size of the data in proot.
 *      pleft Pointer to the left sub node.
 *     pright Pointer to the right sub node.
 * Return value:  proot will return.
 *                NULL would return if function could not create a new node.
 * Caution:       Both pnode1 and pnode2 shall not appear in the same sub-tree.
 *                pnode1 shall not equal to pnode2.
 */
P_TNODE_BY treMergeNodesBY(P_TNODE_BY proot, const void * pitem, size_t size, P_TNODE_BY pleft, P_TNODE_BY pright)
{
	/* Return NULL while pright is a sub tree of pleft or pleft is a sub tree of pright. */
	if (treDescendantBY(pleft, pright) || treDescendantBY(pright, pleft))
		return NULL;
	if (NULL == proot)
		if (NULL == (proot = strCreateNodeD(pitem, size)))
			return NULL;
	proot->ppnode[LEFT]  = pleft;
	proot->ppnode[RIGHT] = pright;
	return proot;
}

/* Function name: treSwapNodesBY
 * Description:   Swap two nodes in a binary tree.
 * Parameters:
 *     proot1 Pointer to the root node that has a certain path to pnode1.
 *     pnode1 Pointer to a node.
 *     proot2 Pointer to the root node that has a certain path to pnode2.
 *     pnode2 Pointer to another node.
 * Return value:  If swapping succeeded, function would return pnode2,
 *                otherwise function would return a NULL.
 * Caution:       proot1 may equal to pnode2.
 *                Both pnode1 and pnode2 shall not appear in the same sub-tree.
 */
P_TNODE_BY treSwapNodesBY(P_TNODE_BY proot1, P_TNODE_BY pnode1, P_TNODE_BY proot2, P_TNODE_BY pnode2)
{
	if (NULL == proot1 || NULL == proot2 ||
		NULL == pnode1 || NULL == pnode2 ||
		treDescendantBY(pnode1, pnode2)   ||
		treDescendantBY(pnode2, pnode1)
		) return NULL; /* T1 contains T2, or T2 contains T1. */
	else
	{
		P_TNODE_BY prt1, prt2;
		prt1 = treGetParentNodeBY(proot1, pnode1);
		prt2 = treGetParentNodeBY(proot2, pnode2);
		if (pnode1 == prt1->ppnode[LEFT]) /* 1L. */
		{
			if (pnode2 == prt2->ppnode[LEFT]) /* 1L.2L */
			{
				prt1->ppnode[LEFT] = pnode2;
				prt2->ppnode[LEFT] = pnode1;
			}
			else /* 1L.2R */
			{
				prt1->ppnode[LEFT]  = pnode2;
				prt2->ppnode[RIGHT] = pnode1;
			}
		}
		else /* 1R. */
		{
			if (pnode2 == prt2->ppnode[LEFT]) /* 1R.2L */
			{
				prt1->ppnode[RIGHT] = pnode2;
				prt2->ppnode[LEFT]  = pnode1;
			}
			else /* 1R.2R */
			{
				prt1->ppnode[RIGHT] = pnode2;
				prt2->ppnode[RIGHT] = pnode1;
			}
		}
		return pnode2;
	}
}

/* Function name: treCopyBY
 * Description:   Copy a binary tree entirely.
 * Parameters:
 *      proot Pointer to the root node of the original binary tree.
 *       size Size of each data in every node of a binary tree.
 * Return value:  A copy of the original binary tree.
 * Caution:       Size of data of every nodes shall be made in the same value.
 */
P_TNODE_BY treCopyBY(P_TNODE_BY proot, size_t size)
{
	_TreeCopy tp;
	QUEUE_L q;
	queInitL(&q);
	tp.pnroot = NULL;
	tp.pquel = &q;
	tp.size = size;
	treTraverseBYLevel(proot, _treCBFCopyTreeNodeB, (size_t)&tp);
	/* As usual, queue q should be empty here.
	 * But if there were an error occurred while new node was allocating in the
	 * callback function, Callback function would return immediately and
	 * quit with a non-empty queue. Be sure to deallocate the queue while
	 * exiting function treCopyBY to prevent a potential memory leak.
	 */
	queFreeL(&q);
	return (P_TNODE_BY)tp.pnroot;
}
