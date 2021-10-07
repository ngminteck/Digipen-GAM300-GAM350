/******************************************************************************/
/*!
\file   Serialization.cpp
\author Goh Zihe
\par    email: zihe.goh\@digipen.edu
\par    Digipen login: zihe.goh
\date   02/02/2020
\brief
Contains definitions required for Serializer & deserializer

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include "pch.h"

using namespace tinyxml2;

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult; }
#endif

auto item_getter = [](void* vec, int idx, const char** out_text)
{
  auto& vector = *static_cast<std::vector<std::string>*>(vec);
  if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
  *out_text = vector.at(idx).c_str();
  return true;
};

Serializer::Serializer(XMLElement &xmlElem) 
  : m_xmlElem(xmlElem),
  m_xmlDoc(*xmlElem.GetDocument())
{
}

void Serializer::Write(const std::string &data)
{
 m_xmlElem.SetAttribute(ASC_SERIAL_NAME, data.c_str());
}

void Serializer::Write(const char * elemName, const Vec2 &vec)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  Append(vec);
}

void Serializer::Write(const char *elemName, const Vec3 &vec)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  Append(vec);
}

void Serializer::Write(const char *elemName, const Vec4 &vec)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  Append(vec);
}

void Serializer::Write(const char * elemName, const Quaternion & vec)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  Append(vec);
}

void Serializer::Write(const char *elemName, const std::string &name)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  Append(name);
}

void Serializer::Write(const char * elemName, std::vector<std::string>& vec)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  assert(lastElement);
  for (auto& filename : vec)
  {
    auto elemNode = m_xmlDoc.NewElement(filename.c_str());
   // elemNode->SetAttribute("val", filename.c_str());
    elemNode->SetAttribute("a", filename.c_str());
    lastElement->InsertEndChild(elemNode);


    //lastElement->SetAttribute("Key", filename.first.c_str());
    //lastElement->SetAttribute("Value", filename.second.c_str());

  }
  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Write(const char* elemName, int val)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  Append(val);
}

void Serializer::Write(const char* elemName, bool val)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  Append(val);
}

void Serializer::Write(const char* elemName, float val)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  Append(val);
}
void Serializer::Write(const char* elemName, double val)
{
    lastElement = m_xmlDoc.NewElement(elemName);
    Append(val);
}
void Serializer::Write(const char *elemName, std::unordered_map<std::string, std::string> &vec)
{
    lastElement = m_xmlDoc.NewElement(elemName);
    Append(vec);
}
void Serializer::Write(const char* elemName, std::unordered_map<int, std::string>& vec)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  assert(lastElement);
  for (auto& filename : vec)
  {
    auto elemNode = m_xmlDoc.NewElement( filename.second.c_str());
    /*elemNode->SetAttribute("val", filename.second);*/
    lastElement->InsertEndChild(elemNode);

  }
  m_xmlElem.InsertEndChild(lastElement);
}
void Serializer::Write(const char* elemName, std::map<std::string, bool>& vec)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  assert(lastElement);
  for (auto& filename : vec)
  {
    auto elemNode = m_xmlDoc.NewElement(filename.first.c_str());
    elemNode->SetAttribute("val",filename.second);

    lastElement->InsertEndChild(elemNode);


    //lastElement->SetAttribute("Key", filename.first.c_str());
    //lastElement->SetAttribute("Value", filename.second.c_str());

  }
  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Write(const char* elemName, std::string key, std::string value)
{
    lastElement = m_xmlDoc.NewElement(elemName);
    Append(key,value);
}

void Serializer:: Write(const char* elemName, std::vector<float>& vec)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  assert(lastElement);
  int i = 0;
  for (auto& val : vec)
  {
    auto elemNode = m_xmlDoc.NewElement("a"); i++;
    elemNode->SetAttribute("val",val  );

    lastElement->InsertEndChild(elemNode);


    //lastElement->SetAttribute("Key", filename.first.c_str());
    //lastElement->SetAttribute("Value", filename.second.c_str());

  }
  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Write(const char* elemName, std::vector<dReal>& vec)
{
    lastElement = m_xmlDoc.NewElement(elemName);
    assert(lastElement);
    int i = 0;
    for (auto& val : vec)
    {
        auto elemNode = m_xmlDoc.NewElement("a"); i++;
        elemNode->SetAttribute("val", val);

        lastElement->InsertEndChild(elemNode);


        //lastElement->SetAttribute("Key", filename.first.c_str());
        //lastElement->SetAttribute("Value", filename.second.c_str());

    }
    m_xmlElem.InsertEndChild(lastElement);
}
void Serializer:: Write(const char* elemName, std::vector<dTriIndex>& vec)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  assert(lastElement);
  int i = 0;
  for (auto& val : vec)
  {
    auto elemNode = m_xmlDoc.NewElement("a"); i++;
    elemNode->SetAttribute("val", val);

    lastElement->InsertEndChild(elemNode);


    //lastElement->SetAttribute("Key", filename.first.c_str());
    //lastElement->SetAttribute("Value", filename.second.c_str());

  }
  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Write(const char* elemName, std::unordered_map < std::string, MsgType> &vec)
{
  lastElement = m_xmlDoc.NewElement(elemName);
  assert(lastElement);
  for (auto& filename : vec)
  {
    auto elemNode = m_xmlDoc.NewElement(filename.first.c_str());
    elemNode->SetAttribute("val", static_cast<int>( filename.second));

    lastElement->InsertEndChild(elemNode);

  }
  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Append(const Vec2 &vec)
{
  assert(lastElement);
  lastElement->SetAttribute("x", vec.x);
  lastElement->SetAttribute("y", vec.y);

  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Append(const Vec3 &vec)
{
  assert(lastElement);
  lastElement->SetAttribute("x", vec.x);
  lastElement->SetAttribute("y", vec.y);
  lastElement->SetAttribute("z", vec.z);

  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Append(const Vec4 &vec)
{
  assert(lastElement);
  lastElement->SetAttribute("x", vec.x);
  lastElement->SetAttribute("y", vec.y);
  lastElement->SetAttribute("z", vec.z);
  lastElement->SetAttribute("w", vec.w);

  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Append(const Quaternion &quat)
{
  assert(lastElement);
  lastElement->SetAttribute("x", quat.x);
  lastElement->SetAttribute("y", quat.y);
  lastElement->SetAttribute("z", quat.z);
  lastElement->SetAttribute("w", quat.w);

  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Append(const std::string &name)
{
  assert(lastElement);
  lastElement->SetAttribute("name", name.c_str());
  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Append(std::vector<std::string>&vec)
{
  assert(lastElement);
  for (auto& filename : vec)
  {
    lastElement->SetAttribute("name", filename.data());
    m_xmlElem.InsertEndChild(lastElement);
  }
}

void Serializer::Append(int val)
{
  assert(lastElement);
  lastElement->SetAttribute("val", val);
  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Append(bool val)
{
  assert(lastElement);
  lastElement->SetAttribute("val", val);
  m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Append(float val)
{
  assert(lastElement);
  lastElement->SetAttribute("val", val);
  m_xmlElem.InsertEndChild(lastElement);
}
void Serializer::Append(double val)
{
    assert(lastElement);
    lastElement->SetAttribute("val", val);
    m_xmlElem.InsertEndChild(lastElement);
}
void Serializer::Append(std::unordered_map<std::string, std::string> &vec)
{
    assert(lastElement);
    for (auto& filename : vec)
    {
        auto elemNode = m_xmlDoc.NewElement(filename.first.c_str());
        elemNode->SetText(filename.second.c_str());

        lastElement->InsertEndChild(elemNode);
       

        //lastElement->SetAttribute("Key", filename.first.c_str());
        //lastElement->SetAttribute("Value", filename.second.c_str());

    }
        m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::Append(const std::string& key, const std::string& value)
{
    assert(lastElement);
    lastElement->SetAttribute("key", key.c_str());
    lastElement->SetAttribute("value", value.c_str());
    m_xmlElem.InsertEndChild(lastElement);
}

void Serializer::EndAppend()
{
  lastElement = nullptr;
}

XMLElement& Serializer::CreateNewChild(const char * name)
{
  XMLElement *elem = m_xmlDoc.NewElement(name);
  m_xmlElem.InsertEndChild(elem);
  return *elem;
}

Deserializer::Deserializer(XMLElement &xmlElem): m_xmlElem(xmlElem)
{
}

void Deserializer::Read(const char *elemName, Vec2 &vec) const
{
  XMLElement *elem = m_xmlElem.FirstChildElement(elemName);

  InternalRead(elem, "x", vec.x);
  InternalRead(elem, "y", vec.y);
}


void Deserializer::Read(const char *elemName, Vec3 &vec) const
{
  XMLElement *elem = m_xmlElem.FirstChildElement(elemName);
  InternalRead(elem, "x", vec.x);
  InternalRead(elem, "y", vec.y);
  InternalRead(elem, "z", vec.z);
}

void Deserializer::Read(const char *elemName, Vec4 &vec) const
{
  XMLElement *elem = m_xmlElem.FirstChildElement(elemName);

  InternalRead(elem, "x", vec.x);
  InternalRead(elem, "y", vec.y);
  InternalRead(elem, "z", vec.z);
  InternalRead(elem, "w", vec.w);
}


//void Deserializer:: Read(const char* elemName, dVector4& vec) const
//{
//  XMLElement* elem = m_xmlElem.FirstChildElement(elemName);
//
//  InternalRead(elem, "x", vec[0]);
//  InternalRead(elem, "y", vec[1]);
//  InternalRead(elem, "z", vec[2]);
//  InternalRead(elem, "w", vec[3]);
//
//}


void Deserializer::Read(const char * elemName, Quaternion &vec) const
{
  XMLElement *elem = m_xmlElem.FirstChildElement(elemName);

  InternalRead(elem, "x", vec.x);
  InternalRead(elem, "y", vec.y);
  InternalRead(elem, "z", vec.z);
  InternalRead(elem, "w", vec.w);
}

void Deserializer::Read(const char *elemName, std::string &name) const
{
  XMLElement *elem = m_xmlElem.FirstChildElement(elemName);
  InternalRead(elem, "name", name);
}

void Deserializer::Read(const char * elemName, int &val) const
{
  XMLElement *elem = m_xmlElem.FirstChildElement(elemName);
  if (!elem)
  {
    LogWarning(elemName, " not found");
    return;
  }
  val = elem->IntAttribute("val");
}

void Deserializer::Read(const char * elemName, bool &val) const
{
  XMLElement *elem = m_xmlElem.FirstChildElement(elemName);
  if (!elem)
  {
    LogWarning(elemName, " not found");
    return;
  }
  val = elem->BoolAttribute("val");
}

void Deserializer::Read(const char * elemName, float &val) const
{
  XMLElement *elem = m_xmlElem.FirstChildElement(elemName);
  if (!elem)
  {
    LogWarning(elemName, " not found");
    return;
  }
  val = elem->FloatAttribute("val");
}
void Deserializer::Read(const char* elemName, double& val) const
{
    XMLElement* elem = m_xmlElem.FirstChildElement(elemName);
    if (!elem)
    {
        LogWarning(elemName, " not found");
        return;
    }
    val = elem->DoubleAttribute("val");
}

void Deserializer::Read(const char * elemName, std::unordered_map<std::string, std::string> &vec) const
{
    XMLElement *elem = m_xmlElem.FirstChildElement(elemName);
    if (!elem)
    {
        LogWarning(elemName, " not found");
        return;
    }
   
    for (auto node = elem->FirstChildElement(); node != nullptr; node = node->NextSiblingElement())
    {
        
        vec[node->Value()] = node->GetText();
     /*   std::cout << node->GetText();
*/
    }

  //  val = elem->FloatAttribute("val");
}
void Deserializer::Read(const char* elemName, std::unordered_map<int, std::string>& vec) const
{
  XMLElement* elem = m_xmlElem.FirstChildElement(elemName);
  if (!elem)
  {
    LogWarning(elemName, " not found");
    return;
  }

  for (auto node = elem->FirstChildElement(); node != nullptr; node = node->NextSiblingElement())
  {
    vec[static_cast<int>( vec.size())] = node->Value();
    /*   std::cout << node->GetText();
*/
  }
}

void Deserializer::Read(const char* elemName, std::map < std::string, bool>&vec) const
{
  XMLElement* elem = m_xmlElem.FirstChildElement(elemName);
  if (!elem)
  {
    LogWarning(elemName, " not found");
    return;
  }

  for (auto node = elem->FirstChildElement(); node != nullptr; node = node->NextSiblingElement())
  {
    vec[node->Value()] = node->BoolAttribute("val");
    /*   std::cout << node->GetText();
*/
  }

}
void Deserializer::Read(const char* elemName, std::vector<float>& vec) const
{
  XMLElement* elem = m_xmlElem.FirstChildElement(elemName);
  if (!elem)
  {
    LogWarning(elemName, " not found");
    return;
  }

  for (auto node = elem->FirstChildElement(); node != nullptr; node = node->NextSiblingElement())
  {
    vec.push_back( node->FloatAttribute("val"));
    /*   std::cout << node->GetText();
*/
  }

}


void Deserializer::Read(const char* elemName, std::vector<dReal>& vec) const
{
    XMLElement* elem = m_xmlElem.FirstChildElement(elemName);
    if (!elem)
    {
        LogWarning(elemName, " not found");
        return;
    }

    for (auto node = elem->FirstChildElement(); node != nullptr; node = node->NextSiblingElement())
    {
        vec.push_back(node->DoubleAttribute("val"));
        /*   std::cout << node->GetText();
    */
    }

}

void Deserializer::Read(const char* elemName, std::vector<dTriIndex>& vec) const
{
  XMLElement* elem = m_xmlElem.FirstChildElement(elemName);
  if (!elem)
  {
    LogWarning(elemName, " not found");
    return;
  }

  for (auto node = elem->FirstChildElement(); node != nullptr; node = node->NextSiblingElement())
  {
   
      vec.push_back(node->IntAttribute("val"));
    
  }
}
void Deserializer:: Read(const char* elemName, std::unordered_map < std::string, MsgType>&vec) const
{

  XMLElement* elem = m_xmlElem.FirstChildElement(elemName);
  if (!elem)
  {
    LogWarning(elemName, " not found");
    return;
  }

  for (auto node = elem->FirstChildElement(); node != nullptr; node = node->NextSiblingElement())
  {
    vec[node->Value()] = static_cast< MsgType>( node->IntAttribute("val"));
    /*   std::cout << node->GetText();
*/
  }

}
void Deserializer:: Read(const char* elemName, std::vector<std::string >&vec) const
{
  XMLElement* elem = m_xmlElem.FirstChildElement(elemName);
  if (!elem)
  {
    LogWarning(elemName, " not found");
    return;
  }


  for (auto node = elem->FirstChildElement(); node != nullptr; node = node->NextSiblingElement())
  {
    vec.push_back(node->Value());
    /*   std::cout << node->GetText();
*/
  }
}



void CapitalizeNames(const char *name, char *buffer)
{
  size_t length = strlen(name);
  buffer[0] = name[0];

  size_t i = 1, j = 1;
  for (i = 1, j = 1; i < length; ++i, ++j)
  {
    if (isupper(name[i]))
    {
      buffer[j] = ' ';
      ++j;
    }
    buffer[j] = name[i];
  }

  buffer[j] = '\0';
}

///////////////////////////////start of file save and loading operations //////////////////////////
XMLfile::XMLfile(const std::string& filename, bool loading)
  : m_filename(filename), m_curr_node(nullptr)
{
  //will load a xml file to xmlDoc if available
  if (loading)
  {
    XMLError err = Load();
    if (err != XML_SUCCESS)
    {
      // ERROR
      LogError("Error xml load unsuccessful");
    }
    //when loading you might not want to save
    Saved = true;
  }
}

XMLfile::XMLfile(const std::pair<const std::string&, bool>& pair)
  : XMLfile(pair.first, pair.second) {}

XMLfile::~XMLfile()
{
  //during loading/reading of file, the file will not auto save
  if (!Saved) Save();
}

XMLError XMLfile::Load()
{
  XMLError result = m_xmlDoc.LoadFile(m_filename.c_str());
  XMLCheckResult(result);

  return XML_SUCCESS;
}

XMLError XMLfile::Save()
{
  XMLError Result = m_xmlDoc.SaveFile(m_filename.c_str());
  XMLCheckResult(Result);

  //saved once before
  Saved = true;
  return XML_SUCCESS;
}

XMLElement * XMLfile::GetDocFirstElement()
{
  XMLElement* tmpElem = m_xmlDoc.FirstChildElement();
  m_curr_node = tmpElem;
  return tmpElem;
}

XMLfileWrite::XMLfileWrite(const std::string & filename, const std::string & startNodeName)
  : XMLfile(filename, false)
{
  m_curr_node = m_xmlDoc.NewElement(startNodeName.c_str());
  m_xmlDoc.InsertFirstChild(m_curr_node);
}

XMLElement * XMLfileWrite::AddAsChild(const char * elmname, bool remainAsParent)
{
  XMLElement* newChild = m_xmlDoc.NewElement(elmname);
  m_curr_node->InsertEndChild(newChild);
  if (!remainAsParent) m_curr_node = newChild;
  return newChild;
}

XMLElement * XMLfileWrite::AddAsSibiling(const char * elmname)
{
  XMLElement* newSib = m_xmlDoc.NewElement(elmname);

  m_curr_node->Parent()->InsertEndChild(newSib);
  m_curr_node = newSib;
  return newSib;
}
/////////////////////////////// end of file save and loading operations //////////////////////////



const float Reflector::speed = 0.01f;

bool Reflector::Reflect(std::string &str) const
{
  return Reflect("Name", str);
}

bool Reflector::Reflect(const char *name, std::string &str, bool pressEnter) const
{
  char buf[MAX_PATH];
  char nameBuf[MAX_PATH];

  strcpy_s<MAX_PATH>(buf, str.c_str());
  CapitalizeNames(name, nameBuf);
  if (!pressEnter || ImGui::InputText(nameBuf, buf, sizeof(buf), ImGuiInputTextFlags_EnterReturnsTrue) )
  {
    str = std::string(buf);
    return true;
  }

 
  return false;
}
bool Reflector::Reflect(const char* name, std::string& str, std::vector<std::string>  const& source, bool remove) const
{

  int index = 0;
  ImGui::BeginChild(name);
  ImGui::Columns(2);
  ImGui::Text(str.c_str());
  ImGui::NextColumn();
  if (Reflect(name, source, &index))
  {
    //we gonig to remove anything tahts behind . , like .png etc
    if (remove)
    {
      str = source[index].substr(0, source[index].find_first_of("."));
    }
    else// just take everything 
    {
      str = source[index];
    }
    return true; 
  }
  ImGui::EndChild();
  return false;
}
//bool Reflector::Reflect(const char* name, std::string const& str, bool pressEnter) const
//{
//  char buf[MAX_PATH];
//  char nameBuf[MAX_PATH];
//
//  strcpy_s<MAX_PATH>(buf, str.c_str());
//  CapitalizeNames(name, nameBuf);
//  ImGui::InputText(nameBuf, buf, sizeof(buf));
//
//  if (!pressEnter || ImGui::IsKeyPressed(VK_RETURN))
//  {
//    //str = std::string(buf);
//    return true;
//  }
//  return false;
//}

bool Reflector::Reflect(const char *name, bool &bValue) const
{
  char buf[MAX_PATH];
  CapitalizeNames(name, buf);
  return ImGui::Checkbox(buf, &bValue);
}

bool Reflector::Reflect(const char * name) const
{
  if (ImGui::Button(name))
    return true;
  return false;
}

bool Reflector::Reflect(const char *name, int &iValue) const
{
  char buf[MAX_PATH];
  CapitalizeNames(name, buf);
  return ImGui::InputInt(buf, &iValue);
}

bool Reflector::Reflect(const char *name, int &iValue, int min, int max) const
{
  char buf[MAX_PATH];
  CapitalizeNames(name, buf);
  return ImGui::SliderInt(buf, &iValue, min, max);
}

bool Reflector::Reflect(const char *name, float &fValue, int precision) const
{
  char buf[MAX_PATH];
  CapitalizeNames(name, buf);
  return ImGui::InputFloat(buf, &fValue, 0.0f, 0.0f, precision);
}

bool Reflector::Reflect(const char *name, float &fValue, float min, float max, float dragSpeed) const
{
  char buf[MAX_PATH];
  CapitalizeNames(name, buf);
  //ImGui::SliderFloat(buf, &fValue, min, max);
  return ImGui::DragFloat(buf, &fValue, dragSpeed, min, max, "%.4f");
}

//bool Reflector::Reflect(const char *name, Vec2 &vec, int precision) const
//{
//  char buf[MAX_PATH];
//  CapitalizeNames(name, buf);
//  return ImGui::InputFloat2(buf, &(vec.x), precision);
//}

bool Reflector::Reflect(const char *name, Vec2 &vec, float min, float max) const
{
  char buf[MAX_PATH];
  CapitalizeNames(name, buf);
  return ImGui::DragFloat2(buf, vec.m, speed, min, max, "%.5f");
  //ImGui::SliderFloat2(buf, &(vec.x), min, max);
}

//bool Reflector::Reflect(const char *name, Vec3 &vec, int precision) const
//{
//  char buf[MAX_PATH];
//  CapitalizeNames(name, buf);
//  return ImGui::InputFloat3(buf, &(vec.x), precision);
//}

bool Reflector::Reflect(const char *name, Vec3 &vec, float min, float max) const
{
  char buf[MAX_PATH];
  CapitalizeNames(name, buf);
  return ImGui::DragFloat3(buf, vec.m, speed, min, max, "%.4f");
}

bool Reflector::ColorPicker(const char * name, float * col, bool alphabar)
{
  const float  EDGE_SIZE = 175.0f; // = int( ImGui::GetWindowWidth() * 0.75f );
  const ImVec2 SV_PICKER_SIZE = ImVec2(EDGE_SIZE, EDGE_SIZE);
  const float  SPACING = ImGui::GetStyle().ItemInnerSpacing.x;
  const float  HUE_PICKER_WIDTH = 20.f;
  const float  CROSSHAIR_SIZE = 7.0f;

  ImGui::PushID(name);

  ImColor color(col[0], col[1], col[2]);
  bool value_changed = false;

  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  // setup

  ImVec2 picker_pos = ImGui::GetCursorScreenPos();

  float hue, saturation, value;
  ImGui::ColorConvertRGBtoHSV(
    color.Value.x, color.Value.y, color.Value.z, hue, saturation, value);

  // draw hue bar

  ImColor colors[] = { ImColor(255, 0, 0),
    ImColor(255, 255, 0),
    ImColor(0, 255, 0),
    ImColor(0, 255, 255),
    ImColor(0, 0, 255),
    ImColor(255, 0, 255),
    ImColor(255, 0, 0) };

  for (int i = 0; i < 6; ++i)
  {
    draw_list->AddRectFilledMultiColor(
      ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING, picker_pos.y + i * (SV_PICKER_SIZE.y / 6)),
      ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING + HUE_PICKER_WIDTH,
        picker_pos.y + (i + 1) * (SV_PICKER_SIZE.y / 6)),
      colors[i],
      colors[i],
      colors[i + 1],
      colors[i + 1]);
  }

  draw_list->AddLine(
    ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING - 2, picker_pos.y + hue * SV_PICKER_SIZE.y),
    ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING + 2 + HUE_PICKER_WIDTH, picker_pos.y + hue * SV_PICKER_SIZE.y),
    ImColor(255, 255, 255));

  // draw alpha bar

  if (alphabar) {
    float alpha = col[3];

    draw_list->AddRectFilledMultiColor(
      ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * SPACING + HUE_PICKER_WIDTH, picker_pos.y),
      ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * SPACING + 2 * HUE_PICKER_WIDTH, picker_pos.y + SV_PICKER_SIZE.y),
      ImColor(0, 0, 0), ImColor(0, 0, 0), ImColor(255, 255, 255), ImColor(255, 255, 255));

    draw_list->AddLine(
      ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * (SPACING - 2) + HUE_PICKER_WIDTH, picker_pos.y + alpha * SV_PICKER_SIZE.y),
      ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * (SPACING + 2) + 2 * HUE_PICKER_WIDTH, picker_pos.y + alpha * SV_PICKER_SIZE.y),
      ImColor(255.f - alpha, 255.f, 255.f));
  }

  // draw color matrix

  {
    const ImU32 c_oColorBlack = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f));
    const ImU32 c_oColorBlackTransparent = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 0.f));
    const ImU32 c_oColorWhite = ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 1.f));

    ImVec4 cHueValue(1, 1, 1, 1);
    ImGui::ColorConvertHSVtoRGB(hue, 1, 1, cHueValue.x, cHueValue.y, cHueValue.z);
    ImU32 oHueColor = ImGui::ColorConvertFloat4ToU32(cHueValue);

    draw_list->AddRectFilledMultiColor(
      ImVec2(picker_pos.x, picker_pos.y),
      ImVec2(picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y),
      c_oColorWhite,
      oHueColor,
      oHueColor,
      c_oColorWhite
    );

    draw_list->AddRectFilledMultiColor(
      ImVec2(picker_pos.x, picker_pos.y),
      ImVec2(picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y),
      c_oColorBlackTransparent,
      c_oColorBlackTransparent,
      c_oColorBlack,
      c_oColorBlack
    );
  }

  // draw cross-hair

  float x = saturation * SV_PICKER_SIZE.x;
  float y = (1 - value) * SV_PICKER_SIZE.y;
  ImVec2 p(picker_pos.x + x, picker_pos.y + y);
  draw_list->AddLine(ImVec2(p.x - CROSSHAIR_SIZE, p.y), ImVec2(p.x - 2, p.y), ImColor(255, 255, 255));
  draw_list->AddLine(ImVec2(p.x + CROSSHAIR_SIZE, p.y), ImVec2(p.x + 2, p.y), ImColor(255, 255, 255));
  draw_list->AddLine(ImVec2(p.x, p.y + CROSSHAIR_SIZE), ImVec2(p.x, p.y + 2), ImColor(255, 255, 255));
  draw_list->AddLine(ImVec2(p.x, p.y - CROSSHAIR_SIZE), ImVec2(p.x, p.y - 2), ImColor(255, 255, 255));

  // color matrix logic

  ImGui::InvisibleButton("saturation_value_selector", SV_PICKER_SIZE);

  if (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])
  {
    ImVec2 mouse_pos_in_canvas = ImVec2(
      ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

    /**/ if (mouse_pos_in_canvas.x < 0) mouse_pos_in_canvas.x = 0;
    else if (mouse_pos_in_canvas.x >= SV_PICKER_SIZE.x - 1) mouse_pos_in_canvas.x = SV_PICKER_SIZE.x - 1;

    /**/ if (mouse_pos_in_canvas.y < 0) mouse_pos_in_canvas.y = 0;
    else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

    value = 1 - (mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1));
    saturation = mouse_pos_in_canvas.x / (SV_PICKER_SIZE.x - 1);
    value_changed = true;
  }

  // hue bar logic

  ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + SPACING + SV_PICKER_SIZE.x, picker_pos.y));
  ImGui::InvisibleButton("hue_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));

  if (ImGui::GetIO().MouseDown[0] && (ImGui::IsItemHovered() || ImGui::IsItemActive()))
  {
    ImVec2 mouse_pos_in_canvas = ImVec2(
      ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

    /**/ if (mouse_pos_in_canvas.y < 0) mouse_pos_in_canvas.y = 0;
    else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

    hue = mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1);
    value_changed = true;
  }

  // alpha bar logic

  if (alphabar) {

    ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + SPACING * 2 + HUE_PICKER_WIDTH + SV_PICKER_SIZE.x, picker_pos.y));
    ImGui::InvisibleButton("alpha_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));

    if (ImGui::GetIO().MouseDown[0] && (ImGui::IsItemHovered() || ImGui::IsItemActive()))
    {
      ImVec2 mouse_pos_in_canvas = ImVec2(
        ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

      /**/ if (mouse_pos_in_canvas.y < 0) mouse_pos_in_canvas.y = 0;
      else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

      float alpha = mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1);
      col[3] = alpha;
      value_changed = true;
    }

  }

  // R,G,B or H,S,V color editor

  color = ImColor::HSV(hue >= 1 ? hue - 10 * 1e-6f : hue, saturation > 0 ? saturation : 10 * 1e-6f, value > 0 ? value : 1e-6f);
  col[0] = color.Value.x;
  col[1] = color.Value.y;
  col[2] = color.Value.z;

  bool widget_used;
  ImGui::PushItemWidth((alphabar ? SPACING + HUE_PICKER_WIDTH : 0) +
    SV_PICKER_SIZE.x + SPACING + HUE_PICKER_WIDTH - 2 * ImGui::GetStyle().FramePadding.x);
  widget_used = alphabar ? ImGui::ColorEdit4(name, col) : ImGui::ColorEdit3(name, col);
  ImGui::PopItemWidth();

  // try to cancel hue wrap (after ColorEdit), if any
  {
    float new_hue, new_sat, new_val;
    ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], new_hue, new_sat, new_val);
    if (new_hue <= 0 && hue > 0) {
      if (new_val <= 0 && value != new_val) {
        color = ImColor::HSV(hue, saturation, new_val <= 0 ? value * 0.5f : new_val);
        col[0] = color.Value.x;
        col[1] = color.Value.y;
        col[2] = color.Value.z;
      }
      else
        if (new_sat <= 0) {
          color = ImColor::HSV(hue, new_sat <= 0 ? saturation * 0.5f : new_sat, new_val);
          col[0] = color.Value.x;
          col[1] = color.Value.y;
          col[2] = color.Value.z;
        }
    }
  }
  ImGui::PopID();
  return value_changed | widget_used;
}

bool Reflector::Reflect(const char *name, Vec4 &vec) const
{
  char buf[MAX_PATH];
  CapitalizeNames(name, buf);
  return ImGui::ColorEdit4(buf, &(vec.x));
}

bool Reflector::Reflect(const char *name, Vec4 &vec, float min, float max) const
{
  char buf[MAX_PATH];
  CapitalizeNames(name, buf);
  return ImGui::DragFloat4(buf, vec.m, speed, min, max);
}

bool Reflector::Reflect(const char * name, Quaternion & vec) const
{
  char buf[MAX_PATH];
  CapitalizeNames(name, buf);
  return ImGui::DragFloat4(buf, vec.m, speed);
}

bool Reflector::Reflect(const char *name, std::vector<std::string> &vec, int *index) const
{
  return ImGui::Combo(name, index, item_getter, &vec, (int)vec.size());
}
bool Reflector::Reflect(const char* name, std::vector<std::string> const & vec, int* index) const
{
  static  const char* curr = NULL;
  if (ImGui::BeginCombo(name, vec[(*index)].c_str()))
  {
    for (int i = 0; i < vec.size(); i++)
    {
      bool selected = (curr == vec[i].c_str()); // You can store your selection however you want, outside or inside your objects
     
      if (ImGui::Selectable(vec[i].c_str(), selected))
      {
        curr = vec[i].c_str();
        *index = i;
        ImGui::EndCombo();
        return true; 
        // preview = bodylist[i].c_str();
      }

      if (selected)
        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
    }
    ImGui::EndCombo();
  }
 // return ImGui::Combo(name, index, item_getter, &vec, (int)vec.size());
  return false;
}



bool Reflector::Reflect(const char* name, std::vector<std::string>& vec) const
{
  ImGui::Text(name);
  if (ImGui::Button("add new line"))
  {
    vec.push_back("");
  }
  int i = 0; 
  for (auto& curr : vec)
  {
    std::string  currName = curr +  std::to_string (i);
    if (Reflect(currName.c_str(), curr))
    {
      int a = 10;
      std::cout << currName <<" ";
      std::cout << curr << std::endl; 
     // return true;
    }
    ImGui::SameLine();
    std::string button_ = "-##reflectionremove" + currName;
    if (ImGui::Button(button_.c_str()))
    {
      vec.erase(vec.begin() + i);
      break;
    }
    i++;
  }
  ImGui::Separator();
  return false; 
}


bool Reflector::Reflect(const char* name, std::vector<std::string>& vec, std::vector<std::string> const   & source, bool remove) const
{
  ImGui::Text(name);
  std::string stuff = "add new line ##" + std::string( name);
  if (ImGui::Button(stuff.c_str() ))
  {
    vec.push_back("");
  }
  int i = 0;
  ImGui::BeginChild(name, ImVec2(0,ImGui::GetWindowSize().y *0.2f));
  ImGui::Columns(3);
  for (auto& curr : vec)
  {
    std::string  currName = std::to_string(i);
    static int index  = 0; 
    ImGui::Text(curr.c_str());
    ImGui::NextColumn();
    if (Reflect(currName.c_str(), source, &index))
    {
      //we gonig to remove anything tahts behind . , like .png etc
      if (remove)
      {
        curr = source[index].substr(0,source[index].find_first_of("."));
      }
      else// just take everything 
      {
        curr = source[index];
      }
    }
    ImGui::NextColumn();
    std::string button_ = "-##reflectionremove" + currName;
    if (ImGui::Button(  button_.c_str()))
    {
      vec.erase(vec.begin()+ i);
      break;
    }
    ImGui::NextColumn();
    i++;
  }
  ImGui::EndChild();
  ImGui::Separator();
  return false;
}

bool Reflector::Reflect(const char* name, std::unordered_map<std::string,std::string>& vec, std::vector<std::string> const& source, bool remove) const
{
  
  ImGui::Text(name);

  if (ImGui::Button("add new line"))
  {
    std::string newName = "blank_"  + std::to_string( vec.size());
    auto it = vec.find(newName);
    if (it == vec.end())
    {
      std::string valName = "noTrack" + std::to_string(vec.size());
      vec.insert({ newName,"noTrack" });
    }
  }
  int i = 0;
  ImGui::BeginChild(name, ImVec2(0, ImGui::GetWindowSize().y * 0.2f));
  ImGui::Columns(4);

  for (auto& [name ,curr] : vec)
  {
    std::string  currName =  name;// + std::to_string(i);
    std::string  a1 = "##" + name;
    static int index = 0;
    if (Reflect(a1.c_str(), currName))
    {
      //change the key name
      auto obj = vec.extract(name);
      obj.key() = currName;
      //putting back the changed value back 
      std::cout << obj.key() << std::endl;
      vec.insert(std::move(obj));
      break;
    }
    ImGui::NextColumn();
    ImGui::Text(curr.c_str());
    ImGui::NextColumn();
    std::string newCurrname = "" + std::to_string(i) ;
   
    if (Reflect(newCurrname.c_str(), source, &index))
    {
     // std::cout << newCurrname << std::endl;
      //we gonig to remove anything tahts behind . , like .png etc
      if (remove)
      {
        curr = source[index].substr(0, source[index].find_first_of("."));
      }
      else// just take everything 
      {
        curr = source[index];
      }
    }
    ImGui::NextColumn();
    std::string button_ = "-##reflectionremove" + currName;
    if (ImGui::Button(button_.c_str()))
    {
      vec.erase(name);
      break;
    }
    ImGui::NextColumn();
    i++;
  }
  ImGui::EndChild();
  ImGui::Separator();
  return false;
}


bool Reflector::Reflect(const char *name, unsigned num)
{
  ImGui::LabelText(name, "%u", num);
  return false;
}


