#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "dom.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace rapidjson;

void dom_traverse_v2::print(const Value &v, std::string key_name)
{
  ss << key_name << ": ";

  if(v.IsString())
  {
    ss << v.GetString() << std::endl;
  }
  else
    if(v.IsInt())
    {
      ss << v.GetInt() << std::endl;
    }
    else
      if(v.IsBool())
      {
  ss << (v.GetBool() ? "true" : "false" ) << std::endl;
      }
      else
  if(v.IsNull())
  {
    ss << "null" << std::endl;
  }
  else
    if(v.IsDouble())
    {
      ss << v.GetDouble() << std::endl;
    }
    else
    {
      ss << "value not exist" << std::endl;
    }
}

void dom_traverse_v2::traverse(Document &d)
{
  std::string path="";

  for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr)
  {
    const Value &v = itr->value;

    if(v.IsArray())
    {
      std::string path="";
      path.append( itr->name.GetString() );
      path.append( "/" );

      traverse_array(v, path);
    }
    else if (v.IsObject())
    {
      std::string path="";
      path.append( itr->name.GetString() );
      path.append( "/" );

      traverse_object(v, path);
    }
    else
    {
      std::string tmp = path;
      path.append( itr->name.GetString() );
      path.append( "/" );
      print(v, path);
      path = tmp;
    }

  }
}

void dom_traverse_v2::traverse_array(const Value &v,std::string path)
{
  std::string object_key = path;
  
  for (Value::ConstValueIterator itr = v.Begin(); itr != v.End(); ++itr)
  {
    const Value& array_item = *itr;
    if(array_item.IsArray())
    {
      traverse_array(array_item,object_key);
    }
    else if(array_item.IsObject())
    {
      traverse_object(array_item,object_key);
    }
    else
    {
  print(array_item, object_key);
    }
  }
}

void dom_traverse_v2::traverse_object(const Value &v,std::string path)
{
  std::string object_key = path;

  for (Value::ConstMemberIterator itr = v.MemberBegin(); itr != v.MemberEnd(); ++itr)
  {
    const Value& v_itr = itr->value;

    if (itr->value.IsObject())
    {
      std::string tmp = object_key;
      object_key.append( itr->name.GetString() );
      object_key.append("/");
      traverse_object(v_itr,object_key);
      object_key = tmp;
    }
    else
      if (itr->value.IsArray())
      {
  object_key.append( itr->name.GetString() );
  object_key.append("/");
  traverse_array(v_itr,object_key);
      }
      else
      {
  std::string tmp = object_key;
  object_key.append( itr->name.GetString() );
  object_key.append("/");
  print(v_itr, object_key);
  object_key = tmp;
      }

  }
}

