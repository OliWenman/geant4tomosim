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
    std::string errorMessage = "\nERROR: " + Command -> GetCommandPath() + " " + input + " ";  
    int errorcounter = 0;
    
    bool flagfound = false;
    int units = 1;
    
    //Loop through the tokens and find the flag
    for (int i = 0; i < tokens.size() ; i++)
    {
        foundFlag = tokens[i].find(flag.name + "=");
        
        //If flag found, seperate the answer from the flag
        if (foundFlag !=std::string::npos)
        {
            answer = tokens[i].substr(flag.name.size() + 1, tokens[i].size());
            flagfound = true;
            
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
                        G4cout << errorMessage << " -> Invalid units";
                        ++errorcounter;
                    }            
                    answer = answer.substr(0, firstbracket);
                }
                else       
                {
                    G4cout << errorMessage << " -> No units given";
                    ++errorcounter;
                }   
            }
            break;
        } 
        else 
        {
            flagfound = false;
        }  
    }
    if (!flagfound)
    {
        G4cout << errorMessage << " -> Invalid flags";
        ++errorcounter;
    }
    
    double value = 0;
    
    try
    {
        value = std::stod(answer)*units;
    }
    catch (const std::invalid_argument& ia)
    {
        G4cout << errorMessage << " -> Invalid number";  
        ++errorcounter;
    }  
    
    if (errorcounter > 0)
    {
        G4cout << "\nUnsuccessful" << G4endl;
        exit(0);
    }
    
    return value;
}

std::string Tokenizer::FindString(Flag flag)
{
    std::size_t foundFlag;
    std::string answer;
    std::string errorMessage = "\nERROR: " + Command -> GetCommandPath() + " " + input + " ";  
    int errorcounter = 0;
    
    bool flagfound = false;
    
    //Loop through the tokens and find the flag
    for (int i = 0; i < tokens.size() ; i++)
    {
        foundFlag = tokens[i].find(flag.name + "=");
        
        //If flag found, seperate the answer from the flag
        if (foundFlag !=std::string::npos)
        {
            answer = tokens[i].substr(flag.name.size() + 1, tokens[i].size());
            flagfound = true;
            
            break;
        } 
        else 
        {
            flagfound = false;
        }  
    }
    if (!flagfound)
    {
        G4cout << errorMessage << " -> Invalid flags";
        ++errorcounter;
    }
    
    if (errorcounter > 0)
    {
        G4cout << "\nUnsuccessful" << G4endl;
        exit(0);
    }
    
    return answer;
}
std::vector<double> Tokenizer::FindArray(Flag flag)
{
    std::size_t foundFlag;
    std::string answer;
    std::string errorMessage = "\nERROR: " + Command -> GetCommandPath() + " " + input + " ";  
    int errorcounter = 0;
    
    bool flagfound = false;
    int units = 1;
    
    std::vector<double> theArray;
    
    //Loop through the tokens and find the flag
    for (int i = 0; i < tokens.size() ; i++)
    {
        foundFlag = tokens[i].find(flag.name + "=");
        
        //If flag found, seperate the answer from the flag
        if (foundFlag !=std::string::npos)
        {
            answer = tokens[i].substr(flag.name.size() + 1, tokens[i].size());
            flagfound = true;
            //G4cout << "answer = " << answer;
            
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
                        G4cout << errorMessage << " -> Invalid units";
                        ++errorcounter;
                    }            
                    answer = answer.substr(0, firstbracket);
                }
                else       
                {
                    G4cout << errorMessage << " -> No units given";
                    ++errorcounter;
                }   
            }
            break;
        } 
        else 
        {
            flagfound = false;
        }  
    }
    if (!flagfound)
    {
        G4cout << errorMessage << " -> Invalid flags";
        ++errorcounter;
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
                G4cout << errorMessage << " -> Invalid number (" << errorcounter << ")" << std::flush;  
                ++errorcounter;
            }
        }
        
        //If the function linspace is detected, create the array
        if(linespaceTrue !=std::string::npos )
        {
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
                G4cout << errorMessage << " -> linspace should only have 3 inputs (" << errorcounter << ")" << std::flush;
                ++errorcounter;  
            }
        } 
        else
        {
            //Else use the array given
            theArray = tempArray;  
        }     
    }
    
    if (errorcounter > 0)
    {
        G4cout << G4endl;
        exit(0);
    }
    
    return theArray;
}
