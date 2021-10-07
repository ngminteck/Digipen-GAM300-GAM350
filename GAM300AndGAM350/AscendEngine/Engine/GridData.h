/******************************************************************************/
/*!
\file   GridData.h
\author Zelin Ong
\par    email: zelin.ong\@digipen.edu
\par    Digipen login: zelin.ong
\date   02/02/2020
\brief
Contains definitions required for GridData

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

//static const int ENABLEFLAG = 0x01;
//static const int NULLFLAG = 0x02;

//the data that will be stored in each cell
struct HexData
{
	bool m_enable : 1;
	bool m_null : 1;
	float m_ht;

	HexData() : m_enable(true), m_null(false), m_ht(0.f) {}

	//toggle will flip status the m_enable flag
	inline void toggleEnable() { m_enable = !m_enable; }
	void Deserialize(tinyxml2::XMLElement* elem);
	void Serialize(tinyxml2::XMLElement* elem);
};

struct PlaneInfo
{
	Vec3 m_midpt;
	Vec3 m_min;
	Vec3 m_max;
	Vec3 m_midptVecX;
	Vec3 m_midptVecY;

	Vec3 m_basis[2];
	Vec3 m_normal;
	Vec2 m_scale;
};

struct GridInfo
{
	unsigned m_row = 10;
	unsigned m_col = 10;
	float m_spacing = 4.f;
	Vec2 m_startpos = Vec2(0.f, 0.f);
	float m_height = 0.f;
	std::string m_name = "DefaultGrid";
};

class Hexgrid
{
	PlaneInfo m_p;
	Vec3 m_dir[6];

	//std::string m_name;
	float m_space;
	float m_vertspace;
	float m_edgelen;
	Vec2 m_startpos;
	float m_height;
	unsigned m_numrow, m_numcol;
	std::vector<HexData> m_data;

	//this plane is for the ray picking check
	//Plane m_p;
	void CreateDirVecToCellCorner(Vec3 dir[6]);
	void CreatePlane();
	//everytime someinfo is change, this function have to be called
	void UpdateInternalInfo();
	inline int rc2i(int row, int col) { return row * m_numcol + col; }
public:
	//methods

	//spacing is the length across 1 edge to the other
	Hexgrid(unsigned numrow = 0, unsigned numcol = 0, float spacing = 2.f,
		Vec2 startpos = Vec2(0.f, 0.f), float height = 0.f, const std::string& name = "DefaultGrid");
	Hexgrid(GridInfo gi) :
		Hexgrid(gi.m_row, gi.m_col, gi.m_spacing, gi.m_startpos, gi.m_height, gi.m_name) {}
	inline HexData& Getdata(int row, int col) { return m_data[row * m_numcol + col]; }
	void DrawGrid();

	//data driven
	//all the function requires the name to be set properly
	void Deserialize();
	void Serialize();


	//helper
	bool boundaryCheck(int row, int col);
	inline Vec2& GetGridPos() { return m_startpos; }
  inline Vec3 GetGridPosInWorld() const { return Vec3(m_startpos.x, m_height, m_startpos.y); }
  void SetWorldPosToGrid(Vec3 const& pos);
	void GetCoordToHexIndex(Vec2 pos, int& row, int& col);
	void GetHexIndexToCoord(int row, int col, Vec2& pos);

	//for raycast choosing
	bool GetChoosenCell(int& row, int& col);
	// this function will check for cell that are valid (aka null) as well 
	//and only return if the cell is selected but not null
	inline bool GetChoosenValidCell(int& row, int& col) { return GetChoosenCell(row, col) && !Getdata(row, col).m_null; }
	void RenderChoosenCell(int row, int col);


	ASC_VarGetSet(std::string, name);
	ASC_VarGetSet(Handle<Hexgrid>, SelfHandle);
};

//also know as grid handle

typedef Handle<Hexgrid> GridHdl;

#define ASC_GRIDMAN GridManager::Inst()
class GridManager : public Singleton<GridManager>
{
	HandleManager<Hexgrid> m_grids;
public:
	GridHdl createGrid(GridInfo gi);
	GridHdl createGridFromFile(const char* name);
	void RemoveGrid(GridHdl ghdl);

	inline std::vector<Hexgrid>& GetAllGrid() { return m_grids.m_objects; }
	inline Hexgrid& GetGrid(GridHdl ghdl) { return *m_grids[ghdl]; }
	inline Hexgrid const& GetGrid(GridHdl ghdl) const { return *m_grids[ghdl]; }
	void DrawAllGrid();
};


//EDITOR STUFF
typedef void(*flipfntype)(Hexgrid& hg, int row, int col);
void Flip(Hexgrid& hg, int row, int col);
void FlipCellNull(Hexgrid& hg, int row, int col);
void FlipCellEnable(Hexgrid& hg, int row, int col);
static flipfntype FlipFn[3] = { Flip, FlipCellNull , FlipCellEnable };


