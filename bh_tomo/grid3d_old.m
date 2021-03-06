%GRID3D class to perform raytracing in 3D with the fast sweeping method
%
%  Usage:
%
%  Create and destroy instance of class
%
%    g = grid3d(par, nthreads)
%    clear g
%
%   Input for instantiation
%    par: struct variable for grid definition with the following fields
%          xmin: origin in X
%          ymin: origin in Y
%          zmin: origin in Z
%          dx: cell size in X
%          dy: cell size in Y
%          dz: cell size in Z
%          nx: number of cells in X
%          ny: number of cells in Y
%          nz: number of cells in Z
%    nthreads: number of threads (optional, default = 1)
%
%  Raytracing
%    [tt] = g.raytrace(s, Tx, Rx, t0)
%    [tt, rays] = g.raytrace(s, Tx, Rx, t0)
%    [tt, rays, L] = g.raytrace(s, Tx, Rx, t0)
%
%   Input
%    g: grid instance
%    s: slowness vector ( nSlowness by 1 )
%    Tx: source coordinates, nTx by 3
%          1st column contains X coordinates, 2nd contains Y coordinates,
%          3rd contains Z coordinates
%    Rx: receiver coordinates, nRx by 3
%          1st column contains X coordinates, 2nd contains Y coordinates,
%          3rd contains Z coordinates
%    t0: source epoch, nTx by 1
%          t0 is optional (0 if not given)
%
%    *** IMPORTANT: Tx or Rx should _not_ lie on (or close to) an external
%                   face of the grid when rays are needed ***
%    *** nTx must be equal to nRx, i.e. each row define one Tx-Rx pair ***
%    *** nSlowness must equal g.nx*g.ny*g.nz ***
%    *** Indexing of slowness values is done by "vectorizing" a 3D array,
%        i.e. if slowness field s is of size (nx,ny,nz), enter s(:) as
%        first argument
%
%
%   Output
%    tt:   vector of traveltimes, nRx by 1
%    rays: cell object containing the matrices of coordinates of the ray
%          paths, nRx by 1.  Each matrix is nPts by 3
%    L:    data kernel matrix (tt = L*s)
%
% -----------
%
% Bernard Giroux
% INRS-ETE
% 2016-02-14

% Copyright (C) 2016 Bernard Giroux
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation; either version 3 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
%


classdef grid3d_old < handle
    properties (Access = private, Hidden = true)
        objectHandle; % Handle to the underlying C++ class instance
    end
    methods
        % Constructor - Create a new C++ class instance
        function this = grid3d_old(varargin)
            this.objectHandle = grid3d_mex('new', varargin{:});
        end

        % Destructor - Destroy the C++ class instance
        function delete(this)
            grid3d_mex('delete', this.objectHandle);
        end

        % raytrace
        function varargout = raytrace(this, varargin)
            [varargout{1:nargout}] = grid3d_mex('raytrace', this.objectHandle, varargin{:});
        end

        % for saving in mat-files
        function s = saveobj(obj)
            s.xmin = grid3d_mex('get_xmin', obj.objectHandle);
            s.ymin = grid3d_mex('get_ymin', obj.objectHandle);
            s.zmin = grid3d_mex('get_zmin', obj.objectHandle);
            s.dx = grid3d_mex('get_dx', obj.objectHandle);
            s.dy = grid3d_mex('get_dy', obj.objectHandle);
            s.dz = grid3d_mex('get_dz', obj.objectHandle);
            s.nx = grid3d_mex('get_nx', obj.objectHandle);
            s.ny = grid3d_mex('get_ny', obj.objectHandle);
            s.nz = grid3d_mex('get_nz', obj.objectHandle);
            s.nthreads = grid3d_mex('get_nthreads', obj.objectHandle);
        end
    end
    methods(Static)
        % for loading from mat-files
        function obj = loadobj(s)
            if isstruct(s)
                obj = grid3d_old(s, s.nthreads);
            else
                error('Wrong input arguments')
            end
        end
    end
end
