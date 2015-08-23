#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <string>
#include <chrono>
#include <future>
#include <unordered_map>
#include <vector>
#include <utility>
#include "SystemData.h"
class Keyboard
{
	const int DELAY_AMOUNT = 50; //Delay by 50ms for repeat input.
	const int NUM_KEYS = 52; //Number of keys in the string.
	const int ITERATE_AMOUNT = 2; //Amount to iterate per left or right input.
	const std::string enKeys = "A b c d e f g h i j k l m n o p q r s t u v w x y z"; //The initial string.
	const unsigned int JUMP_LIMIT = 20; //Arbitrary number for max loops.
public:
	/*  Keyboard - Initializes the 2d keyboard for jumping.
		@param std::string - the language of the keys to set the keyboard to.
		@param std::vector<SystemData*> the vector containing all loaded game systems.
	*/
	Keyboard(std::string language, std::vector<SystemData*> systemRoot);
	/*  Input - evaluates the input into the keyboard.
		@param InputConfig* config - evaluates a button press to the users abstracted input.
		@param Input input, the object containgin the button press
	*/
	virtual bool input(InputConfig* config, Input input);
	/*  Render - renders the keyboard to the screen.
		@param const Eigen::Affine3f& - The percentage value of the parents size.
	*/
	virtual void render(const Eigen::Affine3f& parentTrans);
	//iterateRight - Moves the keyboard selection to the right
	void iterateRight();
	//iterateLeft - Moves the keybaord selection to the left.
	void iterateLeft();
	//Resets the keyboard's internal states to the defaults, does not modify internal game data.
	void reset();
	/*  goToKey - Goes to the selected key from the keyboard.
		@param FileData* - The pointer of the currently selected system.
		@return FileData* - the game to set the user's cursor to.
	*/
	FileData* goToKey(FileData* systemRoot);
	/*  getKeys - Gets the string containing the currently displayed keyboard keys.
		@return std::string& - the currently displayed keys by the keyboard.
	*/
	std::string& getKeys() { return keys; };
	/*  setKeys - Sets the keyboard to the string provided.
		@param std::string - the value to set the keyboard to.
	*/
	void setKeys(std::string) { this->keys = keys; };
	/*  isOpen - returns if the keyboard is open or not.
		@return bool - true if the keyboard is open, false otherwise.
	*/
	bool isOpen() { return mKeyboardIsOpen; };
	/*  setKeyboardIsOpen - sets the keyboard state if its open or not.
		@param bool - the value to set the keyboard's display state to.
	*/
	void setKeyboardIsOpen(bool state);
	/*  getPosisition - Gets the keyboard's display position.
		@param int - the position that the keyboard is currently in.
	*/
	int getPosition() { return position; };
	/*  setPosition - sets the position of the keyboard.
		@param unsigned int - the location to set the keyboard\
	*/
	void setPosition(unsigned int position) { this->position = position; };
	/*  changed - Returns if the keyboard state has changed or not and needs a render cache update.
		@return - true if the keyboard's internal state has changed, false otherwise.
	*/
	bool changed() { return hasChanged; };
	/*  getPrevChoice - Returns the first chare of the users initial choice.
		@return char - the value of the users initial choice.
	*/
	char getPrevChoice() { return prevChoice; };
	//General Destructor
	~Keyboard();
private:
	/*  populateHashTable - Indexes all systems and their games data.
		@param std::vector<SystemData*> - The vector containing all the systems and their respective games.
		@return bool - true if done, used for asychronous indexing.
	*/
	bool populateHashTable(std::vector<SystemData*> systemRoot);
	//The future for if the hash table is done initializing.
	std::future<bool> hashTableInitResult;
	//Returns true if the hashTable is done.
	bool hashTableDone;
	//Hashs a system FileData* to a pointer to an alphabet/numerical char that goes to the start of that number/char.
	std::unordered_map < std::string , int> systemIndex;
	//Hash's the pointer to a file of the first character that appears in that file and returns its character.
	std::vector<std::unordered_map < char, FileData* const>* > gameIndex;
	//Current position of the user's selection on the keyboard.
	int position;
	//If the keyboard has changed and needs an update.
	bool hasChanged;
	//the current keys that the keyboard is displaying.
	std::string keys;
	//If the keyboard is open or not.
	bool mKeyboardIsOpen;
	//The current language that the keyboard is initialized to. (en, gb, etc)
	std::string language;
	//The previous choice of the user on the keyboard.
	char prevChoice;
	//Timer that records when the keyboard was open to avoid duplicate input.
	std::chrono::system_clock::time_point open;
};

#endif
