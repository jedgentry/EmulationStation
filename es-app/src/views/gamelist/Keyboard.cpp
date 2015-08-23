#include "Keyboard.h"

Keyboard::Keyboard(std::string language, std::vector<SystemData*> systemRoot)
{
	hashTableInitResult = std::async(std::launch::async, &Keyboard::populateHashTable, this, systemRoot);
	//populateHashTable(systemRoot);
	keys.reserve(NUM_KEYS); //Avoid deep copy.
	systemIndex.reserve(50);
	this->language = language;
	if (language == "en" || language == "gb")
	{
		keys = enKeys;
	}
	mKeyboardIsOpen = false;
	position = 0;
	hasChanged = false;
	hashTableDone = false;
	prevChoice = ' ';
}

void Keyboard::setKeyboardIsOpen(bool state)
{
	if (mKeyboardIsOpen == false && state == true)
	{
		open = std::chrono::system_clock::now();
		mKeyboardIsOpen = true;
	}
	if (mKeyboardIsOpen == true && state == false)
	{
		if (std::chrono::duration_cast<std::chrono::milliseconds>((std::chrono::system_clock::now() - open)).count() 
			< DELAY_AMOUNT)
		{
			return;
		}
		else
		{
			mKeyboardIsOpen = false;
			reset();
		}
	}
}

bool Keyboard::input(InputConfig* config, Input input)
{
	return true;
}

void Keyboard::render(const Eigen::Affine3f& parentTrans)
{
	return;
}

void Keyboard::iterateLeft()
{
	keys[position] = tolower(keys[position]);
	position -= ITERATE_AMOUNT;
	if (position < 0)
	{
		position = NUM_KEYS + position;
	}
	hasChanged = true;
	keys[position] = toupper(keys[position]);
}

void Keyboard::iterateRight()
{
	keys[position] = tolower(keys[position]);
	position += ITERATE_AMOUNT;
	if (position >= NUM_KEYS)
	{
		position = NUM_KEYS % position;
	}
	hasChanged = true;
	keys[position] = toupper(keys[position]);
}

FileData* Keyboard::goToKey(FileData* systemRoot)
{
	if (hashTableDone == false) //No need to get the value just need it done (blocking).
	{
		hashTableInitResult.get();
		hashTableDone = true;
	}
	try
	{
		prevChoice = keys[position];
		return gameIndex.at(systemIndex.at(systemRoot->getPath().string()))->at(keys[position]);
	}
	catch (const std::out_of_range&)
	{
		return nullptr; //User selected something out of range.
	}
}

void Keyboard::reset()
{
	position = 0;
	hasChanged = true;
	if (language == "en" || language == "gb")
	{
		keys = enKeys;
	}
}

Keyboard::~Keyboard()
{
	gameIndex.clear();
	systemIndex.clear();
}

///////////////////////
// Private Functions //
///////////////////////
bool Keyboard::populateHashTable(std::vector<SystemData*> systemRoot)
{
	for (unsigned int j = 0; j < systemRoot.size(); j++)
	{
		systemIndex.insert(std::make_pair(systemRoot[j]->getStartPath(), j));
		if (systemRoot[j]->getRootFolder()->getChildren().size() > 0)
		{
			std::unordered_map<char, FileData* const>* temp = new std::unordered_map<char, FileData* const>();
			gameIndex.push_back(temp);
			gameIndex.at(j)->insert(std::make_pair(systemRoot[j]->getRootFolder()->getChildren()[0]->getCleanName()[0],
				systemRoot[j]->getRootFolder()->getChildren()[0]));
			char current = systemRoot[j]->getRootFolder()->getChildren()[0]->getCleanName()[0];
			for (unsigned int i = 0; i < systemRoot[j]->getRootFolder()->getChildren().size(); i++)
			{
				if (systemRoot[j]->getRootFolder()->getChildren()[i]->getCleanName()[0] > current)
				{
					gameIndex.at(systemIndex.at(systemRoot[j]->getStartPath()))->insert(std::make_pair(systemRoot[j]->getRootFolder()->getChildren().at(i)->getCleanName()[0],
						systemRoot[j]->getRootFolder()->getChildren().at(i)));
					current++;
				}
				else if (current > 'z') //Cut short, we're done indexing.
					break;
			}
		}
	}
	return true;
}
