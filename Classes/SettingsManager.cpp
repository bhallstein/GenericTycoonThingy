#include "SettingsManager.hpp"

SettingsManager::SettingsManager()
{

}

SettingsManager::~SettingsManager()
{

}

int SettingsManager::load(bool defaults)
{
	int error = 0;
	std::string fileName;

	if(!defaults) fileName = "Data/settings.xml";
	else fileName = "Data/defaultsettings.xml";

	//First load the settings.xml, and populate settings objects from here
	// Create an empty property tree object
    using boost::property_tree::ptree;
    ptree pt;

    // Load the XML file into the property tree. If reading fails
    // (cannot open file, parse error), an exception is thrown.
    read_xml(fileName, pt);

    BOOST_FOREACH(ptree::value_type &cat,pt.get_child("settings"))
	{
		BOOST_FOREACH(ptree::value_type &set,pt.get_child("settings").get_child(cat.first))
		{
			//Create a setting
			Setting *x = new Setting();

			//Populate it with properties from the XML
			x->type = set.first;
			x->category = cat.first;
			x->code = set.second.get<int>("<xmlattr>.code");
			x->detect = set.second.get<bool>("<xmlattr>.detect",false);
			x->dev = set.second.get<bool>("<xmlattr>.dev",false);
			x->displayText = set.second.get<std::string>("displaytext","");
			x->enabled = set.second.get<bool>("<xmlattr>.enabled",true);
			x->key = set.second.get<std::string>("key","");
			x->range[0] = set.second.get<int>("min",0);
			x->range[1] = set.second.get<int>("max",0);
			x->tooltip = set.second.get<std::string>("tooltip","");
			x->value = set.second.get<std::string>("value","");

			//loop through all children of this setting, looking for 'arg' and 'option' keys
			BOOST_FOREACH(ptree::value_type &key,cat.second.get_child(set.first))
			{
				if(key.first == "arg")
					x->args.push_back(key.second.get_value(""));
				if(key.first == "option")
					x->options.push_back(key.second.get_value(""));
			}

			//add to the Settings Map, with a Key for lookups
			setMap[x->key] = *x;

			x->~Setting(); //destroy the setting?
		}
	}

	return error;
	
	// Get the filename and store it in the m_fil variable.
    // Note that we construct the path to the value by separating
    // the individual keys with dots. If dots appear in the keys,
    // a path type with a different separator can be used.
    // If the debug.filename key is not found, an exception is thrown.
    
	//settingsFile = pt.get<std::string>("debug.filename");

    // Get the debug level and store it in the m_level variable.
    // This is another version of the get method: if the value is
    // not found, the default value (specified by the second
    // parameter) is returned instead. The type of the value
    // extracted is determined by the type of the second parameter,
    // so we can simply write get(...) instead of get<int>(...).
    
	//m_level = pt.get("debug.level", 0);

    // Iterate over the debug.modules section and store all found
    // modules in the m_modules set. The get_child() function
    // returns a reference to the child at the specified path; if
    // there is no such child, it throws. Property tree iterators
    // are models of BidirectionalIterator.
    
	/*BOOST_FOREACH(ptree::value_type &v,
            pt.get_child("debug.modules"))
        m_modules.insert(v.second.data());*/


	//populate valid arguments list

	//check command line arguments against the valid list (arguments should be an object so we can assess type; only as simply as whether or not it takes a second param)
}


//Setting class
Setting::Setting()
{

}

Setting::~Setting()
{

}

/*

//setup our acceptable switches
  //map<string,int> numberWang; //used to convert the typed switches to numbers for the switch statement below //this is now part of the class.
  //This data should now be populated from settings.xml
  //program settings
  numberWang["/i"] = 1;
  numberWang["/o"] = 2;
  numberWang["/verbose"] = 3;
  numberWang["/v"] = 3;
  //10+ prefix lists
  numberWang["/local"] = 11;
  numberWang["/sql"] = 12;
  //20+ syntax settings
  numberWang["/t"] = 21;
  numberWang["/types"] = 21;
  numberWang["/typed"] = 21;
  //30+ Synapse settings
  numberWang["/e"] = 31;
  numberWang["/env"] = 31;
  numberWang["/environment"] = 31;

map<string,string> ProcessArgs(int argc, char *argv[]) //simply processes the command line arguments against a valid list
{
  map<string,string> theMap; //this is only used to return command line params?
  string theKey;
  string theValue;

  //first add argv[0] - always the executable path
  theKey = "ExePath";
  theValue = argv[0];

  SetMap[theKey] = theValue;
  
  //add input sdl file path as first arg if not a switch
  if(argc > 1)
  {
    if(argv[1][0] != '/')
    {
      theKey = "iPath";
      theValue = argv[1];

      SetMap[theKey] = theValue;
    }
  }
  
  //add output sdl file path as second arg if not a switch
  if(argc > 2)
  {
    if(argv[2][0] != '/')
    {
      theKey = "oPath";
      theValue = argv[2];

      theMap[theKey] = theValue;
    }
  }
  
  //command line switches (only check for switches (and then their values); we take no regular arguments)
  for (int i = 1;i < argc;i++)
  {
    //see if the parameter is a switch; ignore it if not
    if(argv[i][0] == '/') //check for the leading slash to identify
    {
      switch(numberWang[argv[i]])
      {
        case 1: //i
          theKey = "iPath";
          theValue = argv[++i]; //the parameter follows the switch
          break;
        case 2: //o
          theKey = "oPath";
          theValue = argv[++i]; //the parameter follows the switch
          break;
        case 3: //Verbose, d
          theKey = "Verbose";
          theValue = "1"; //This is an on/off switch
          break;
        case 21: //typed, t, types
          theKey = "Typed";
          theValue = "1"; //This is an on/off switch
          break;
        case 11: //local - Local prefixes
          theKey = "Local";
          theValue = argv[++i]; //the parameter follows the switch
          break;
        case 12: //sql - SQL prefixes
          theKey = "SQL";
          theValue = argv[++i]; //the parameter follows the switch
          break;
        case 31: //sql - SQL prefixes
          theKey = "Environment";
          theValue = argv[++i]; //the parameter follows the switch
          break;
        default:
          //not accepted param
          theKey = "";
      }

      if(theKey != "") //make sure the key isn't empty
      {
        //add it to the Map
        theMap[theKey] = theValue;
      }
    }
    else
    {
      //invalid param - do nothing atmo; maybe Verbose/logging output in future?
    }
  }

  return theMap;
}*/
