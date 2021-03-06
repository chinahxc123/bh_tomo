//
//  Node3Dc.h
//
//  Created by Bernard Giroux on 08-04-24.
//
//  Modified by Benoit Larouche on 12-07-12
//  	: now support parallel raytracing from many source points
//  	  on the same 3D grid simultaneously, using OpenMP.
//  	  Velocity model is sampled at the primary nodes
//  	  Secondary nodes velocity is linearly interpolated
//

//
// Copyright (C) 2012 Bernard Giroux, Benoît Larouche.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __NODE3DC_H__
#define __NODE3DC_H__

#include <cmath>
#include <limits>

#include "Node.h"

template<typename T1, typename T2>
class Node3Dc : public Node<T1> {
public:
    Node3Dc(const int nt) :
	nThreads(nt), 
    tt(0),
	x(0.0f), y(0.0f), z(0.0f),
    gridIndex(std::numeric_limits<T2>::max()),
    nodeParent(0),
    cellParent(0),
    owners(0)
    {
		tt = new T1[nt];
		nodeParent = new T2[nt];
		cellParent = new T2[nt];
		
		for ( size_t n=0; n<nt; ++n ) {
			tt[n] = std::numeric_limits<T1>::max();
			nodeParent[n] = std::numeric_limits<T2>::max();
			cellParent[n] = std::numeric_limits<T2>::max();
		}
	}

    Node3Dc(const T1 t, const T1 xx, const T1 yy, const T1 zz, const int nt,
			const int i) :
	nThreads(nt), 
    tt(0),
    x(xx), y(yy), z(zz),
    gridIndex(std::numeric_limits<T2>::max()),
    nodeParent(0),
    cellParent(0),
    owners(0)
    {
		tt = new T1[nt];
		nodeParent = new T2[nt];
		cellParent = new T2[nt];
		
		for ( size_t n=0; n<nt; ++n ) {
			tt[n] = std::numeric_limits<T1>::max();
			nodeParent[n] = std::numeric_limits<T2>::max();
			cellParent[n] = std::numeric_limits<T2>::max();
		}
        tt[i]=t;
    }
	
	Node3Dc(const Node3Dc<T1,T2>& node) :
	nThreads(node.nThreads), 
    tt(0),
	x(node.x), y(node.y), z(node.z),
    gridIndex(node.gridIndex),
    nodeParent(0),
    cellParent(0),
    owners(node.owners)
    {
		tt = new T1[nThreads];
		nodeParent = new T2[nThreads];
		cellParent = new T2[nThreads];
		
		for ( size_t n=0; n<nThreads; ++n ) {
			tt[n] = node.tt[n];
			nodeParent[n] = node.nodeParent[n];
			cellParent[n] = node.cellParent[n];
		}
	}

	~Node3Dc() {
		delete [] tt;
		delete [] nodeParent;
		delete [] cellParent;
	}

    // Sets the vectors to the right size of threads and initialize it
    void reinit(const int n) {	
        tt[n] = std::numeric_limits<T1>::max();
        nodeParent[n] = std::numeric_limits<T2>::max();
        cellParent[n] = std::numeric_limits<T2>::max();
    }
    
    T1 getTT(const int n) const { return tt[n]; }
    void setTT(const T1 t, const int n ) { tt[n] = t; }

	void setXYZindex(const T1 xx, const T1 yy, const T1 zz, const T2 index) {
		x=xx; y=yy; z=zz; gridIndex = index;  }

    T1 getX() const { return x; }
    void setX(const T1 xx) { x = xx; }
    
    T1 getY() const { return y; }
    void setY(const T1 yy) { y = yy; }

    T1 getZ() const { return z; }
    void setZ(const T1 zz) { z = zz; }
    
    T2 getGridIndex() const { return gridIndex; }
    void setGridIndex(const T2 index) { gridIndex = index; }
    
    T2 getNodeParent(const int n) const { return nodeParent[n]; }
    void setnodeParent(const T2 index, const int n) { nodeParent[n] = index; }
    
    T2 getCellParent(const int n) const { return cellParent[n]; }
    void setCellParent(const T2 index, const int n) { cellParent[n] = index; }
    
    void pushOwner(const T2 o) { owners.push_back(o); }    
    const std::vector<T2>& getOwners() const { return owners; }
    
    T1 getDistance( const Node3Dc<T1,T2>& node ) const {
        return sqrt( (x-node.x)*(x-node.x) + (y-node.y)*(y-node.y) + (z-node.z)*(z-node.z) );
    }

    T1 getDistance( const sxyz<T1>& node ) const {
        return sqrt( (x-node.x)*(x-node.x) + (y-node.y)*(y-node.y) + (z-node.z)*(z-node.z) );
    }
	
	// operator to test if same location
	bool operator==( const sxyz<T1>& node ) const {
		return fabs(x-node.x)<small && fabs(y-node.y)<small && fabs(z-node.z)<small;
	}

    size_t getSize() const {
        return sizeof(int) + nThreads*sizeof(T1) + 3*sizeof(T1) +
        (1+2*nThreads)*sizeof(T2) + owners.size() * sizeof(T2);
    }

	int getDimension() const { return 3; }
	
private:
	int nThreads;
    T1 *tt;                          // travel time for the multiple source points
    T1 x;                            // x coordinate [km]
    T1 y;							 // y coordinate [km]
    T1 z;                            // z coordinate [km]
    T2 gridIndex;               // index of this node in the list of the grid
    T2 *nodeParent;             // index of parent node of the ray for each thread
    T2 *cellParent;             // index of cell traversed by the ray for each thread
    std::vector<T2> owners;     // indices of cells touching the node
	
};


#endif
