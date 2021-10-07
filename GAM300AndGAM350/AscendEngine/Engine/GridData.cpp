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

#include "pch.h"

#define CALROW(i, col) (i / col)
#define CALCOL(i, col) (i % col)
//static const float SIN60 = sin(ASC_DEG2RAD(60));
//static const float SIN30 = sinf(ASC_DEG2RAD(30));
//static const float COS30 = cosf(ASC_DEG2RAD(30));

static const std::string Gridpath = GetCurrentDir() + ASC_GRID_FLD;

//helpers
//inline Vec3 vec223(Vec2 vec, float h = 0.f) { return Vec3{ vec.x, h, vec.y }; }
//inline Vec2 vec322(Vec3 vec) { return Vec2{vec.x, vec.z}; }
//
//inline void rotby30(Vec3& vec) 
//{
//  float x = vec.x * COS30 - vec.z * SIN30;
//  float z = vec.x * SIN30 + vec.z * COS30;
//  vec.x = x;
//  vec.z = z;
//}
//
//inline void rotbyneg_30(Vec3& vec)
//{
//  float x = vec.x * COS30 + vec.z * SIN30;
//  float z = vec.x * -SIN30 + vec.z * COS30;
//  vec.x = x;
//  vec.z = z;
//}

//end of helpers

void Hexgrid::SetWorldPosToGrid(Vec3 const & pos)
{
  m_height = pos.y;
  m_startpos = vec322(pos);
  //we will need to update grid
  UpdateInternalInfo();
}

void Hexgrid::GetCoordToHexIndex(Vec2 pos, int & row, int & col)
{
  /* rhombus implementation */
  row = static_cast<int>(roundf((pos.y - m_startpos.y) / m_vertspace));
  col = static_cast<int>(roundf(((pos.x - m_startpos.x) / (2.f * m_space)) - 0.5f * row));
}

//5 / 7 rating
void Hexgrid::GetHexIndexToCoord(int row, int col, Vec2 & pos)
{
  /* rhombus implementation */
  pos.y = static_cast<float>(m_startpos.y + (row * m_vertspace));
  //col * (2 * m_space) + (row * m_space)
  pos.x = static_cast<float>(m_startpos.x + (m_space * (2 * col + row)));
}

//this function will return true if collided with current grid plane and cells
bool Hexgrid::GetChoosenCell(int & row, int & col)
{
  //create a plane first
  Plane pl(m_p.m_normal, m_p.m_basis[0], m_p.m_basis[1], m_p.m_scale);
  //get ray from cam
  Vec3 campos;// = ASC_GMAN.GetCamera().GetPos();
  Vec3 rayend;
  ASC_GMAN.GetCamera().GetRayFromMouse(campos, rayend);
  Vec3 raydir = rayend - campos;// = ASC_GMAN.GetCamera().GetForward();
  //Vec3 raydir = ASC_GMAN.GetCamera().GetForward();
  raydir = raydir.Normalize();
  float t;
  //check for ray intersection from cam
  bool collided = RayToPlane(m_p.m_min, campos, raydir, pl, t);

  //get the position that the plane collided with the ray
  Vec3 pos = campos + raydir * t;
  //ASC_LINERENDER.AddSphere(pos, 0.5f);

  GetCoordToHexIndex(vec322(pos), row, col);
  return collided && boundaryCheck(row, col);// && !Getdata(row, col).m_null;
}

void Hexgrid::RenderChoosenCell(int row, int col)
{
  Vec2 cenpt;
  GetHexIndexToCoord(row, col, cenpt);
  Vec3 cenpt3{ cenpt.x, m_height + Getdata(row, col).m_ht, cenpt.y };

  Vec3 pts[6];
  for (int i = 0; i < 6; ++i)
    pts[i] = cenpt3 + m_dir[i];

  //for rendering choosen cell 
  for (int i = 0; i < 3; ++i)
    ASC_LINERENDER.AddLine(pts[i], pts[i + 3]);
}

bool Hexgrid::boundaryCheck(int row, int col)
{
  return row >= 0 && row < (int)m_numrow && col >= 0 && col < (int)m_numcol;
}

void Hexgrid::CreateDirVecToCellCorner(Vec3 dir[6])
{
  const Vec3 rightvec3{ (float)m_edgelen, 0.f, 0.f }, upvec3{ 0.f, 0.f, (float)m_edgelen };
  Vec3 rightNE = rightvec3, rightSE = rightvec3;
  rotby30(rightNE);
  rotbyneg_30(rightSE);

  dir[0] = rightNE;
  dir[1] = upvec3;
  dir[2] = -rightSE;
  dir[3] = -rightNE;
  dir[4] = -upvec3;
  dir[5] = rightSE;
}

void Hexgrid::CreatePlane()
{
	//create a estimate plane from the data
  //all calculation is in the AUTODESK BOOK
  unsigned start[2] = { 0, 0 };
  unsigned xend[2] = { 0, m_numcol - 1 };
  unsigned yend[2] = { m_numrow - 1, 0 };
  //get the basis vector
  Vec2 tmpstart, tmpendx, tmpendy;
  //currently just use the start and end of the of the grid to simulate the vectors
  GetHexIndexToCoord(start[0], start[1], tmpstart);
  GetHexIndexToCoord(xend[0], xend[1], tmpendx);
  GetHexIndexToCoord(yend[0], yend[1], tmpendy);

  //calculate the end of tmpstart
  tmpstart.y = tmpstart.y - m_edgelen;
  tmpstart.x = tmpstart.x - m_space * 2.0f;
  Vec3 tmpstart3 = vec223(tmpstart, m_height + Getdata(start[0], start[1]).m_ht);

  Vec2 sideOffset(2.f * -m_space / 3.0f, m_edgelen);
  tmpendx -= sideOffset;
  tmpendy += sideOffset;
  Vec3 bx = vec223(tmpendx, m_height + Getdata(xend[0], xend[1]).m_ht) - tmpstart3;
  Vec3 by = vec223(tmpendy, m_height + Getdata(yend[0], yend[1]).m_ht) - tmpstart3;
  m_p.m_basis[0] = bx.Normalize();
  m_p.m_basis[1] = by.Normalize();

  //take the start and end of the grid
  m_p.m_min = tmpstart3;// -m_p.m_basis[0] * m_space - m_p.m_basis[1] * m_space;
  m_p.m_normal = m_p.m_basis[1].Cross(m_p.m_basis[0]).Normalize();

  //m_p.m_scale.x = (bx + m_p.m_basis[0] * m_space * 2).Magnitude();
  m_p.m_scale.x = (bx).Magnitude();
  //m_p.m_scale.y = (by + m_p.m_basis[1] * m_space * 2).Magnitude();
  m_p.m_scale.y = (by).Magnitude();

  //calculate the position of the plane
  m_p.m_midptVecX = m_p.m_min + m_p.m_basis[0] * m_p.m_scale.x;
  m_p.m_midptVecY = m_p.m_min + m_p.m_basis[1] * m_p.m_scale.y;
  m_p.m_max = -m_p.m_min
    + (m_p.m_midptVecX)
    + (m_p.m_midptVecY);

  m_p.m_midpt =  0.5 * (m_p.m_min + m_p.m_max);
}

void Hexgrid::UpdateInternalInfo()
{
  //this means we only create plane if there is data 
  if (m_data.size()) CreatePlane();
  CreateDirVecToCellCorner(m_dir);
}

Hexgrid::Hexgrid(unsigned numrow, unsigned numcol, float spacing, Vec2 startpos, float height, const std::string& name) :
  m_space(spacing / 2.f), m_vertspace(m_space * 2.f * SIN60), m_edgelen(m_space / SIN60),
  m_startpos(startpos),
  m_numrow(numrow), m_numcol(numcol), m_height(height),
  m_data(m_numrow * m_numcol),
  m_name(name)
{
  //test
  std::mt19937 sowed;
  size_t sz = m_data.size();
  //float t = 0.f;
  for (size_t i = 0; i < sz; ++i)
  {
    //unsigned numh = (sowed() % 16);
    //float h = numh * 0.01f; 
    //float h = +t;
	m_data[i].m_ht = 0.f;// h;
    //t += 0.01f;
  }

  UpdateInternalInfo();
}

void Hexgrid::DrawGrid()
{
  Vec3 start{m_startpos.x, 0.f, m_startpos.y};

  //get ray cast pos
  int c_row = -1, c_col = -1;
  GetChoosenCell(c_row, c_col);

  size_t sz = m_data.size();
  for (size_t i = 0; i < sz; ++i)
  {
    Vec2 cenpt;
    unsigned ui = static_cast<unsigned>(i);
    int curr_row = CALROW(ui, m_numcol);
    int curr_col = CALCOL(ui, m_numcol);
    GetHexIndexToCoord(curr_row, curr_col, cenpt);
    //call the add line function from graphics
    Vec3 cenpt3{ cenpt.x, m_height + m_data[i].m_ht, cenpt.y};

    //let us check to see if we need to render the cell
    if (m_data[i].m_null)
    {
      //draw an x and continue
      static const float mdfiy = 0.01f;
      static const Vec3 MYZAXIS = Vec3::ZAXIS * mdfiy;
      static const Vec3 MYXAXIS = Vec3::XAXIS * mdfiy;
      ASC_LINERENDER.AddLine(cenpt3 - MYZAXIS, cenpt3 + MYZAXIS);
      ASC_LINERENDER.AddLine(cenpt3 - MYXAXIS, cenpt3 + MYXAXIS);
      continue;
    }

    Vec3 pts[6];
    for (int i = 0; i < 6; ++i)
      pts[i] = cenpt3 + m_dir[i];


    for (int i = 0; i < 6; ++i)
    {
      ASC_LINERENDER.AddLine(pts[i], pts[(i + 1) % 6]);
    }

    //if the cell is not enable we draw another pattern
    if (!m_data[i].m_enable)
    {
      for (int i = 0; i < 6; ++i)
      {
        ASC_LINERENDER.AddLine(cenpt3 + 0.5 * m_dir[i], cenpt3 + 0.5 * m_dir[(i + 1) % 6]);
      }
    }

    //for rendering choosen cell 
    //if (c_row == curr_row && c_col == curr_col)
    //  RenderChoosenCell(curr_row, curr_col);

    //ASC_LINERENDER.AddLine(start, cenpt3);
    start = cenpt;
  }

  //draw the plane for vizualization
  ASC_LINERENDER.AddLine(m_p.m_min, m_p.m_midptVecX);
  ASC_LINERENDER.AddLine(m_p.m_min, m_p.m_midptVecY);
  ASC_LINERENDER.AddLine(m_p.m_max, m_p.m_midptVecX);
  ASC_LINERENDER.AddLine(m_p.m_max, m_p.m_midptVecY);
  //draw the normal
  ASC_LINERENDER.AddLine(m_p.m_midpt, m_p.m_midpt + m_p.m_normal);
}

static const char* HGattri[] =
{
  "NAME",
  "ROW",
  "COL",
  "HALF_SPACE",
  "POSX",
  "POSY",
  "HEIGHT",
};

void Hexgrid::Deserialize()
{
  using namespace tinyxml2;
  //will need to load the respective file name
  XMLfile rfile(Gridpath + m_name + ".xml", true);
  XMLElement* head = rfile.GetDocFirstElement();
  //assign the name and the name should actually came from

  //if head is not empty probably it is the node Hexgrid...
  //so we get the child of it and deserialize everyone of it
  if (head)
  {
    //get the attributes 
    m_numrow = head->UnsignedAttribute(HGattri[1]);
    m_numcol = head->UnsignedAttribute(HGattri[2]);
    m_space = head->FloatAttribute(HGattri[3]);
    m_startpos.x = head->FloatAttribute(HGattri[4]);
    m_startpos.y = head->FloatAttribute(HGattri[5]);
    m_height = head->FloatAttribute(HGattri[6]);
    //calculate all the other variables from m_space
    m_vertspace = m_space * 2.f * SIN60;
    m_edgelen = m_space / SIN60;

    //reserve set of data for later
    m_data = std::vector<HexData>(m_numrow * m_numcol);

    head = head->FirstChildElement();
    unsigned int i = 0;
    //if everything is alright it should start from index zero to the max
    while (head)
    {
      m_data[i].Deserialize(head);
      head = head->NextSiblingElement();
      ++i;
    }
  }
  else
  {
    Log(m_name, "Grid deserialize fail");
  }

  UpdateInternalInfo();
}

void Hexgrid::Serialize()
{
  using namespace tinyxml2;
  std::string path = Gridpath + m_name + ".xml";
  XMLfileWrite file(path, "Hexgrid");
  XMLElement* head = file.m_curr_node;
  head->SetAttribute(HGattri[1], m_numrow);
  head->SetAttribute(HGattri[2], m_numcol);
  head->SetAttribute(HGattri[3], m_space);
  head->SetAttribute(HGattri[4], m_startpos.x);
  head->SetAttribute(HGattri[5], m_startpos.y);
  head->SetAttribute(HGattri[6], m_height);

  size_t sz = m_data.size();
  for (size_t i = 0; i < sz; ++i)
  {
    XMLElement* child = file.m_xmlDoc.NewElement("cell");
    m_data[i].Serialize(child);
    head->InsertEndChild(child);
  }
}

static const char* HDatt[]
{
  "ENABLE",
  "NULL",
  "HEIGHT",
};

void HexData::Deserialize(tinyxml2::XMLElement * elem)
{
  m_enable = elem->BoolAttribute(HDatt[0]);
  m_null = elem->BoolAttribute(HDatt[1]);
  m_ht = elem->FloatAttribute(HDatt[2]);
}

void HexData::Serialize(tinyxml2::XMLElement * elem)
{
  elem->SetAttribute(HDatt[0], m_enable);
  elem->SetAttribute(HDatt[1], m_null);
  elem->SetAttribute(HDatt[2], m_ht);
}

/***********************************Gridmanager stuff*******************************************/

GridHdl GridManager::createGrid(GridInfo gi)
{
  return m_grids.GetNewObject(gi);
}

GridHdl GridManager::createGridFromFile(const char * name)
{
  GridHdl ghdl = m_grids.GetNewObject();
  m_grids[ghdl]->Setname(name);
  m_grids[ghdl]->Deserialize();

  return ghdl;
}

void GridManager::RemoveGrid(GridHdl ghdl)
{
  m_grids.ReleaseObj(ghdl);
}

void GridManager::DrawAllGrid()
{
  for (Hexgrid& hg : m_grids.m_objects)
  {
    hg.DrawGrid();
  }
}


//EDITOR STUFF
void Flip(Hexgrid& hg, int row, int col) {}
void FlipCellNull(Hexgrid& hg, int row, int col) { hg.Getdata(row, col).m_null = !hg.Getdata(row, col).m_null; }
void FlipCellEnable(Hexgrid& hg, int row, int col) { hg.Getdata(row, col).m_enable = !hg.Getdata(row, col).m_enable; }
