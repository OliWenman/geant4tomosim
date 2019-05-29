#include "Tokenizer.hh"
#include <algorithm>

Tokenizer::Tokenizer(std::string Input, G4UIcommand* command)
{
    std::string delimiter = " ";
    
    std::vector<std::string> result;

    std::size_t current = 0;
    std::size_t p = Input.find_first_of(delimiter, 0);

    while (p != std::string::npos)
    {
        result.emplace_back(Input, current, p - current);
        current = p + 1;
        p = Input.find_first_of(delimiter, current);
    }

    result.emplace_back(Input, current);
    
    input = Input;
    tokens = result;
     
    Command = command;
}

Tokenizer::~Tokenizer()
{

}

double Tokenizer::FindDouble(Flag flag)
{
    std::size_t foundFlag;
    std::string answer;
    int errorcounter = 0;
    
    int units = 1;
 
    int i = flag.parameter;
 
    foundFlag = tokens[i].find(flag.name + "=");
    
    //If flag found, seperate the answer from the flag
    if (foundFlag !=std::string::npos)
    {
        answer = tokens[i].substr(flag.name.size() + 1, tokens[i].size());
    }
    else
    {

        if (tokens[i].find('=') != std::string::npos)
        {
            throw InvalidFlag();
        }
        else if (tokens[i].find('=')  == std::string::npos && flag.optional)
        {
            answer = tokens[i];
        }
    }  
    
    //Check if the flag has units
    if(flag.hasUnits)
    {
        size_t firstbracket = answer.find("[");
        size_t endbracket = answer.find("]");

        //If it does then check if theres units in the string
        if(firstbracket !=std::string::npos && endbracket !=std::string::npos)
        {
            std::string unitkey = answer.substr(firstbracket + 1, endbracket - firstbracket -1);
            if(flag.units.count(unitkey))
            {
                units = units * flag.units[unitkey];          
            }
            else
            {
                throw InvalidUnits();
            }            
            answer = answer.substr(0, firstbracket);
        }
        else       
        {
            throw InvalidUnits();
        }   
    }
    
    double value = 0;
    
    value = std::stod(answer)*units;
    
    return value;
}

std::string Tokenizer::FindString(Flag flag)
{
    std::size_t foundFlag;
    std::string answer;
    int errorcounter = 0;
    int i = flag.parameter;
    
    foundFlag = tokens[i].find(flag.name + "=");
    
    //If flag found, seperate the answer from the flag
    if (foundFlag !=std::string::npos)
    {
        answer = tokens[i].substr(flag.name.size() + 1, tokens[i].size());
    } 
    else 
    {
        if (tokens[i].find('=') != std::string::npos)
        {
            throw InvalidFlag();
        }
        else if (tokens[i].find('=')  == std::string::npos && flag.optional)
        {
            answer = tokens[i];
        }
        //throw InvalidFlag();
    }  
    
    return answer;
}
doubleArray Tokenizer::FindArray(Flag flag)
{
    std::size_t foundFlag;
    std::string answer;
    double units = 1.;
    
    std::vector<double> theArray;
     
    int i = flag.parameter;
     
    foundFlag = tokens[i].find(flag.name + "=");
    if (foundFlag == std::string::npos)
    {
        if (tokens[i].find('=') != std::string::npos)
        {
            throw InvalidFlag();
        }
        else if (tokens[i].find('=')  == std::string::npos && flag.optional)
        {
            answer = tokens[i];
        }
    }
    else
    {
        //If flag found, seperate the answer from the flag
        answer = tokens[i].substr(flag.name.size() + 1, tokens[i].size());
    }
                    
    //Check if the flag has units
    if(flag.hasUnits)
    {
        size_t firstbracket = answer.find("[");
        size_t endbracket = answer.find("]");

        //If it does then check if theres units in the string at the end
        if(firstbracket !=std::string::npos && endbracket !=std::string::npos && endbracket == answer.size()-1)
        {
            std::string unitkey = answer.substr(firstbracket + 1, endbracket - firstbracket -1);
            if(flag.units.count(unitkey))
            {
                units = units * flag.units[unitkey];          
            }
            else
            {           
                throw InvalidUnits();
            }            
            answer = answer.substr(0, firstbracket);
        }
        else       
        {                    
            throw InvalidUnits();
        }   
    } 
    
    //Checking for brackets to define array
    size_t firstbracket = answer.find("(");
    size_t endbracket = answer.find(")");
    size_t linespaceTrue = answer.find("linspace");
    
    size_t arraysize = 0;
    
    //If brackets found
    if(firstbracket !=std::string::npos && endbracket !=std::string::npos)
    {
        //New string without brackets
        std::string arraystring = answer.substr(firstbracket+1, endbracket - firstbracket-1);
        
        arraysize = std::count(arraystring.begin(), arraystring.end(), ',') + 1;
    
        std::vector<std::string> stringArray(arraysize);
        std::vector<double> tempArray(arraysize);
        
        int element = 0;
        
        //Seperate numbers in string using a ','
        for (int i = 0 ; i <= arraystring.length(); i++)
        {
            if (arraystring[i] != ',')
            {
                if(arraystring[i] != '(' && arraystring[i] != ')')
                {
                    stringArray[element] += arraystring[i];
                }
            }
            else
            {
                element++;
            }
        }
        
        //Convert strings to doubles
        for (int i = 0; i < tempArray.size(); i++)
        {
            try
            {
                tempArray[i] = std::stod(stringArray[i])*units;
            }
            catch (const std::invalid_argument& ia)
            {
                throw InvalidArray();
            }
        }
        
        //If the function linspace is detected, create the array
        if(linespaceTrue !=std::string::npos)
        {
            if (linespaceTrue != 0 || answer.find("linspace(") == std::string::npos) { throw InvalidLinspace();} 
        
            if (tempArray.size() == 3)
            {
                double nElements = tempArray[2]/units;
                double iNum = tempArray[0]/units;
                double eNum = tempArray[1]/units;
            
                double dStep = ((eNum - iNum)/nElements);
                std::vector<double> tempArray2(nElements);
                for (int i = 0; i < nElements ; i++){tempArray2[i] = (dStep * (i + 1))*units;}
            
                theArray= tempArray2;
            }
            else
            {
                throw InvalidLinspace();
            }
        } 
        else
        {
            //Else use the array given
            theArray = tempArray;  
        }     
    }
    
    doubleArray array;
    array.values = new double[theArray.size()];
    array.size = theArray.size();
    
    for (int i = 0; i < theArray.size() ; i++)
    {
        array.values[i] = theArray[i];
    }
    
    return array;
}
