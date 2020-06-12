#ifndef OPERATIVE_NAMES_SET_FILE_H
#define OPERATIVE_NAMES_SET_FILE_H



#include "OperativeNamesSet.h"
#include "Parser.h"



namespace HoI4
{

class OperativeNamesSet::Details: commonItems::parser
{
  public:
	Details();
	std::unique_ptr<OperativeNamesSet> getOperativeNamesSet(std::istream& theStream);

  private:
	std::unique_ptr<OperativeNamesSet> operativeNamesSet;
};

} // namespace HoI4



#endif // OPERATIVE_NAMES_SET_FILE_H