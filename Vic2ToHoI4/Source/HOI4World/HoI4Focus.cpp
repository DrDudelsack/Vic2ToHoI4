#include "HoI4Focus.h"
#include "HoI4Localisation.h"
#include "ParserHelpers.h"



HoI4Focus::HoI4Focus(std::istream& theStream)
{
	registerKeyword(std::regex("id"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::singleString idString(theStream);
		id = idString.getString();
	});
	registerKeyword(std::regex("icon"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::singleString iconString(theStream);
		icon = iconString.getString();
	});
	registerKeyword(std::regex("text"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::singleString textString(theStream);
		text = textString.getString();
	});
	registerKeyword(std::regex("mutually_exclusive"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::stringOfObject mutuallyExclusiveString(theStream);
		mutuallyExclusive = mutuallyExclusiveString.getString();
	});
	registerKeyword(std::regex("bypass"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::stringOfObject bypassString(theStream);
		bypass = bypassString.getString();
	});
	registerKeyword(std::regex("x"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::singleInt xPosInt(theStream);
		xPos = xPosInt.getInt();
	});
	registerKeyword(std::regex("y"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::singleInt yPosInt(theStream);
		yPos = yPosInt.getInt();
	});
	registerKeyword(std::regex("relative_position_id"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::singleString relativePositionIdString(theStream);
		relativePositionId = relativePositionIdString.getString();
	});
	registerKeyword(std::regex("cost"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::singleInt costInt(theStream);
		cost = costInt.getInt();
	});
	registerKeyword(std::regex("available_if_capitulated"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::singleString availableIfCapitulatedString(theStream);
		availableIfCapitulated = (availableIfCapitulatedString.getString() == "yes");
	});
	registerKeyword(std::regex("available"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::stringOfObject availableString(theStream);
		available = availableString.getString();
	});
	registerKeyword(std::regex("cancel_if_invalid"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::singleString cancelIfInvalidString(theStream);
		cancelIfInvalid = cancelIfInvalidString.getString();
	});
	registerKeyword(std::regex("continue_if_invalid"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::singleString continueIfInvalidString(theStream);
		continueIfInvalid = continueIfInvalidString.getString();
	});
	registerKeyword(std::regex("complete_tooltip"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::stringOfObject completeTooltipString(theStream);
		completeTooltip = completeTooltipString.getString();
	});
	registerKeyword(std::regex("completion_reward"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::stringOfObject completionRewardString(theStream);
		completionReward = completionRewardString.getString();
	});
	registerKeyword(std::regex("ai_will_do"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::stringOfObject aiWillDoString(theStream);
		aiWillDo = aiWillDoString.getString();
	});
	registerKeyword(std::regex("prerequisite"), [this](const std::string& unused, std::istream& theStream) {
		commonItems::stringOfObject prerequisiteString(theStream);
		prerequisites.push_back(prerequisiteString.getString());
	});

	parseStream(theStream);
}


std::shared_ptr<HoI4Focus> HoI4Focus::makeCustomizedCopy(const std::string& country) const
{
	auto newFocus = std::make_shared<HoI4Focus>(*this);

	newFocus->text = id;
	newFocus->id += country;

	if (newFocus->relativePositionId != "")
	{
		newFocus->relativePositionId += country;
	}
	if (newFocus->mutuallyExclusive != "")
	{
		customizeMutualExclusion(newFocus, country);
	}

	newFocus->prerequisites.clear();
	for (auto prerequisite: prerequisites)
	{
		customizePrerequisite(newFocus, prerequisite, country);
	}

	return newFocus;
}


std::shared_ptr<HoI4Focus> HoI4Focus::makeTargetedCopy(const std::string& country,
	 const std::string& target,
	 HoI4::Localisation& localisation) const
{
	auto newFocus = std::make_shared<HoI4Focus>(*this);

	newFocus->text = id + target;
	localisation.copyFocusLocalisations(newFocus->id, newFocus->text);
	localisation.updateLocalisationText(newFocus->text, "$TARGET", target);
	localisation.updateLocalisationText(newFocus->text + "_desc", "$TARGET", target);
	newFocus->id += country + target;

	if (newFocus->relativePositionId != "")
	{
		newFocus->relativePositionId += country;
	}
	if (newFocus->mutuallyExclusive != "")
	{
		customizeMutualExclusion(newFocus, country);
	}

	newFocus->prerequisites.clear();
	for (auto prerequisite: prerequisites)
	{
		customizePrerequisite(newFocus, prerequisite, country);
	}

	return newFocus;
}


void HoI4Focus::customizeMutualExclusion(std::shared_ptr<HoI4Focus> newFocus, const std::string& country) const
{
	// have to account for several foci in one mututal exclusion, so need to look for occurences of " focus" and insert
	// country before that
	unsigned int stringPosition = 0;
	do
	{
		auto& mutuallyExclusive = newFocus->mutuallyExclusive;
		int focusPosition = mutuallyExclusive.find(" focus = ", stringPosition);
		if (focusPosition != std::string::npos)
		{
			int spacePosition = mutuallyExclusive.find(" ", focusPosition + 9);
			if (spacePosition == std::string::npos)
			{
				spacePosition = mutuallyExclusive.find("}", focusPosition + 9);
			}
			mutuallyExclusive.insert(spacePosition, country);
			stringPosition = focusPosition + country.size() + 9;
		}
		else
		{
			stringPosition = mutuallyExclusive.size();
		}
	} while (stringPosition < newFocus->mutuallyExclusive.size());
}


void HoI4Focus::customizePrerequisite(std::shared_ptr<HoI4Focus> newFocus,
	 std::string& prerequisite,
	 const std::string& country) const
{
	// have to account for several foci in one prerequisite, so need to look for occurences of " focus" and insert
	// country before that
	unsigned int stringPosition = 0;
	do
	{
		int focusPosition = prerequisite.find(" focus = ", stringPosition);
		if (focusPosition != std::string::npos)
		{
			auto spacePosition = prerequisite.find(' ', focusPosition + 9);
			prerequisite.insert(spacePosition, country);
			stringPosition = focusPosition + country.size() + 9;
		}
		else
		{
			stringPosition = prerequisite.size();
		}
	} while (stringPosition < prerequisite.size());
	newFocus->prerequisites.push_back(prerequisite);
}