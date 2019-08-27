#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

bool userMode = false;

class Node {
	/*
		Generic node to represent family tree.
	*/
public:
	string mName;
	bool mGender;//True-Male, False-Female

	Node* ptr2father;
	Node* ptr2mother;
	Node* ptr2spouse;
	Node* ptr2FirstChild;
	Node* ptr2NextSibling;

	int heightOfNode; // can be used to find the relation between two persons !!! (EXT)

	Node();
	Node(string personName, bool personGender);
};

Node::Node() {
	/*
		Setting default state of Node.
	*/
	this->mName = "";
	this->mGender = false;

	this->ptr2father = NULL;
	this->ptr2mother = NULL;
	this->ptr2spouse = NULL;
	this->ptr2FirstChild = NULL;
	this->ptr2NextSibling = NULL;

	this->heightOfNode = 0;
}

Node::Node(string personName, bool personGender) {
	this->mName = personName;
	this->mGender = personGender;

	this->ptr2father = NULL;
	this->ptr2mother = NULL;
	this->ptr2spouse = NULL;
	this->ptr2FirstChild = NULL;
	this->ptr2NextSibling = NULL;

	this->heightOfNode = 0;
}

class familyTree {
	/*
		To implement various functionalities of family tree.
	*/
public:
	Node* head;


	familyTree();

	//Helper-Queries
	Node* traverseChild(Node* ptr, string personName);
	Node* traverseSiblings(Node* ptr, string personName);
	Node* searchPerson(string personName);
	void makeCouple(string directChild, string spouseName);


	//User-Queries
	void addChild(string parentName, string childName, bool childGender);
	vector<string> getRelationShip(string personName, string relation);
};

familyTree::familyTree() {
	Node* kingShan = new Node("King-Shan", true);
	Node* queenAnga = new Node("Queen-Anga", false);

	kingShan->ptr2spouse = queenAnga;
	kingShan->heightOfNode = 1;
	queenAnga->ptr2spouse = kingShan;
	queenAnga->heightOfNode = 1;

	head = queenAnga;
}

Node* familyTree::traverseSiblings(Node* ptr, string personName) {
	/*
		Seach all the siblings
	*/

	ptr = ptr->ptr2NextSibling;
	while (ptr != NULL) {
		Node* temp = NULL;//to assign value in the else-if statement to avoid unnecesary dublicate calls.
		if (ptr->mName == personName)
			return ptr;
		else if(ptr->ptr2spouse != NULL && ptr->ptr2spouse->mName == personName)//spouse exist &&  check if the spouse is the searched name !
			return ptr->ptr2spouse;
		else if ((temp = traverseChild(ptr, personName)) != NULL)
			return temp;
		else
			ptr = ptr->ptr2NextSibling;
	}

	return NULL;
} 

Node * familyTree::traverseChild(Node * ptr, string personName) {
	/*
		Search all the children.
	*/

	ptr = ptr->ptr2FirstChild;
	while (ptr != NULL) {
		Node* temp = NULL;//to assign value in the else-if statement to avoid dublicate calls.

		if (ptr->mName == personName)
			return ptr;
		else if (ptr->ptr2spouse != NULL && ptr->ptr2spouse->mName == personName)//spouse exist &&  check if the spouse is the searched name !
			return ptr->ptr2spouse;
		else if ((temp = traverseSiblings(ptr, personName)) != NULL)
			return temp;
		else
			ptr = ptr->ptr2FirstChild;
	}

	return NULL;
}

Node * familyTree::searchPerson(string personName) {
	/*
		If person with given name is present it returns the Node.
	*/

	Node* temp = NULL; //to assign value in the else-if statement to avoid dublicate calls.

	if (head->mName == personName)
		return head;
	else if ((temp = traverseSiblings(head, personName)) != NULL)
		return temp;
	else if ((temp = traverseChild(head, personName)) != NULL)
		return temp;

	return NULL;
}

void familyTree::makeCouple(string directChild, string spouseName) {
	/*
		To create a new Node for the spouse and connect it with the spouse and vice-versa
	*/


	Node* ptrDirectChild = searchPerson(directChild);
	Node* ptrSpouse = new Node(spouseName, !ptrDirectChild->mGender);

	ptrDirectChild->ptr2spouse = ptrSpouse;
	ptrSpouse->ptr2spouse = ptrDirectChild;


	ptrSpouse->heightOfNode = ptrDirectChild->heightOfNode;

	return;
}

void familyTree::addChild(string parentName, string childName, bool childGender) {
	/*
		childName is added as firstChild of parentName |OR| childName is
		added as sibling to the last child of personName
	*/

	Node* temp = new Node(childName, childGender);

	Node* ptrParent = searchPerson(parentName);

	if (ptrParent == NULL) {// If parent not found
		cout << "PERSON_NOT_FOUND" << endl;
		return;
	}

	if (ptrParent->mGender && userMode) {//if parent is a male
		cout << "CHILD_ADDITION_FAILED" << endl;
		return;
	}

	/*
		Keeping the logic to add child in father & mother separate to
		consider the divorce situations where each parents may have different
		childs after marrying again to somebody.

	*/


	//Connecting Parents->Child

	//-> mother
	if (ptrParent->ptr2FirstChild == NULL)//adding as firstChild
		ptrParent->ptr2FirstChild = temp;

	else {// adding as sibling to the last child
		Node* pChild = ptrParent->ptr2FirstChild;

		while (pChild->ptr2NextSibling != NULL)
			pChild = pChild->ptr2NextSibling;

		pChild->ptr2NextSibling = temp;

	}

	//-> father
	Node* pSpouse = ptrParent->ptr2spouse;
	if (pSpouse->ptr2FirstChild == NULL)//adding as firstChild
		pSpouse->ptr2FirstChild = temp;

	//connecting Child -> Parents
	temp->ptr2mother = ptrParent;
	temp->ptr2father = pSpouse;

	temp->heightOfNode = ptrParent->heightOfNode + 1;

	if (userMode)
		cout << "CHILD_ADDITION_SUCCEEDED" << endl;

	return;
}

vector<string> familyTree::getRelationShip(string personName, string relation) {
	/*
		1. Paternal-Uncle
		2. Maternal-Uncle
		3. Paternal-Aunt
		4. Maternal-Aunt
		5. Sister-In-Law
		6. Brother-In-Law
		7. Son
		8. Daughter
		9. Siblings
	*/
	vector<string> queryRes;

	Node* ptrCurr = searchPerson(personName);
	if (ptrCurr == NULL) {
		queryRes.push_back("PERSON_NOT_FOUND");
		return queryRes;
	}

	if (relation == "Paternal-Uncle") {// Father's Brothers
		Node* pFather = ptrCurr->ptr2father;

		Node* pGFather = NULL;
		if (pFather != NULL)
			pGFather = pFather->ptr2father;

		if (pGFather != NULL) {
			Node* pFirstChild = pGFather->ptr2FirstChild;// Go to the first child of grandfather

			if (pFirstChild != NULL) {
				if (pFirstChild->mGender == true && pFirstChild != pFather)
					queryRes.push_back(pFirstChild->mName);

				Node * pNextSiblings = pFirstChild->ptr2NextSibling;// getting all the successive siblings
				while (pNextSiblings != NULL) {
					if (pNextSiblings->mGender == true && pNextSiblings != pFather)// if its a male and not same as the father
						queryRes.push_back(pNextSiblings->mName);

					pNextSiblings = pNextSiblings->ptr2NextSibling;
				}
			}
		}
	}

	else if (relation == "Maternal-Uncle") {// Mother's Brothers
		Node* pMother = ptrCurr->ptr2mother;

		Node* pGMother = NULL;
		if (pMother != NULL)
			pGMother = pMother->ptr2mother;

		if (pGMother != NULL) {
			Node* pFirstChild = pGMother->ptr2FirstChild;// Go to the first child of grandMother

			if (pFirstChild != NULL) {
				if (pFirstChild->mGender == true)
					queryRes.push_back(pFirstChild->mName);

				Node * pNextSiblings = pFirstChild->ptr2NextSibling;// getting all the successive siblings
				while (pNextSiblings != NULL) {
					if (pNextSiblings->mGender == true)
						queryRes.push_back(pNextSiblings->mName);

					pNextSiblings = pNextSiblings->ptr2NextSibling;
				}

			}
		}
	}

	else if (relation == "Paternal-Aunt") {// Father's Sisters
		Node* pFather = ptrCurr->ptr2father;

		Node* pGFather = NULL;
		if (pFather != NULL)
			pGFather = pFather->ptr2father;

		if (pGFather != NULL) {
			Node* pFirstChild = pGFather->ptr2FirstChild;// Go to the first child of grandfather

			if (pFirstChild != NULL) {
				if (pFirstChild->mGender == false)
					queryRes.push_back(pFirstChild->mName);

				Node * pNextSiblings = pFirstChild->ptr2NextSibling;// getting all the successive siblings
				while (pNextSiblings != NULL) {
					if (pNextSiblings->mGender == false)
						queryRes.push_back(pNextSiblings->mName);

					pNextSiblings = pNextSiblings->ptr2NextSibling;
				}
			}
		}
	}

	else if (relation == "Maternal-Aunt") {// Mother's Sisters
		Node* pMother = ptrCurr->ptr2mother;

		Node* pGMother = NULL;
		if (pMother != NULL)
			pGMother = pMother->ptr2mother;

		if (pGMother != NULL) {
			Node* pFirstChild = pGMother->ptr2FirstChild;// Go to the first child of grandMother

			if (pFirstChild != NULL) {
				if (pFirstChild->mGender == false && pFirstChild != pMother)
					queryRes.push_back(pFirstChild->mName);

				Node * pNextSiblings = pFirstChild->ptr2NextSibling;// getting all the successive siblings
				while (pNextSiblings != NULL) {
					if (pNextSiblings->mGender == false && pNextSiblings != pMother)// should not be same as the mother
						queryRes.push_back(pNextSiblings->mName);

					pNextSiblings = pNextSiblings->ptr2NextSibling;
				}

			}
		}
	}

	else if (relation == "Sister-In-Law") {// Spouse's Sisters & Wives of Siblings

		//Spouse's Sisters
		Node* pSpouse = ptrCurr->ptr2spouse;

		Node* pMother = NULL;
		if (pSpouse != NULL)
			pMother = pSpouse->ptr2mother;

		if (pMother != NULL) {
			Node* pFirstChild = pMother->ptr2FirstChild;//Go to the first Child of the mother

			if (pFirstChild != NULL) {
				if (pFirstChild->mGender == false && pSpouse != pFirstChild)
					queryRes.push_back(pFirstChild->mName);

				Node * pNextSiblings = pFirstChild->ptr2NextSibling;
				while (pNextSiblings != NULL) {
					if (pNextSiblings->mGender == false && pSpouse != pNextSiblings)
						queryRes.push_back(pNextSiblings->mName);

					pNextSiblings = pNextSiblings->ptr2NextSibling;
				}
			}
		}

		//Wives of Siblings
		pMother = ptrCurr->ptr2mother;

		if (pMother != NULL) {
			Node* pFirstChild = pMother->ptr2FirstChild;

			if (pFirstChild != NULL) {
				if (pFirstChild->mGender == true && pFirstChild != ptrCurr && pFirstChild->ptr2spouse != NULL)
					queryRes.push_back(pFirstChild->ptr2spouse->mName);

				Node * pNextSiblings = pFirstChild->ptr2NextSibling;
				while (pNextSiblings != NULL) {
					if (pNextSiblings->mGender == true && pNextSiblings != ptrCurr && pNextSiblings->ptr2spouse != NULL)
						queryRes.push_back(pNextSiblings->ptr2spouse->mName);

					pNextSiblings = pNextSiblings->ptr2NextSibling;
				}
			}
		}
	}

	else if (relation == "Brother-In-Law") {// Spouse's Brothers & Husband's of siblings

		//Spouse's Brothers
		Node* pSpouse = ptrCurr->ptr2spouse;

		Node* pMother = NULL;
		if (pSpouse != NULL)
			pMother = pSpouse->ptr2mother;

		if (pMother != NULL) {
			Node* pFirstChild = pMother->ptr2FirstChild;//Go to the first Child of the mother

			if (pFirstChild != NULL) {
				if (pFirstChild->mGender == true && pSpouse != pFirstChild)
					queryRes.push_back(pFirstChild->mName);

				Node * pNextSiblings = pFirstChild->ptr2NextSibling;
				while (pNextSiblings != NULL) {
					if (pNextSiblings->mGender == true && pSpouse != pNextSiblings)
						queryRes.push_back(pNextSiblings->mName);

					pNextSiblings = pNextSiblings->ptr2NextSibling;
				}
			}
		}

		//Husband's of siblings
		pMother = ptrCurr->ptr2mother;

		if (pMother != NULL) {
			Node* pFirstChild = pMother->ptr2FirstChild;

			if (pFirstChild != NULL) {
				if (pFirstChild->mGender == false && pFirstChild != ptrCurr && pFirstChild->ptr2spouse != NULL)
					queryRes.push_back(pFirstChild->ptr2spouse->mName);

				Node * pNextSiblings = pFirstChild->ptr2NextSibling;
				while (pNextSiblings != NULL) {
					if (pNextSiblings->mGender == false && pNextSiblings != ptrCurr && pNextSiblings->ptr2spouse != NULL)
						queryRes.push_back(pNextSiblings->ptr2spouse->mName);

					pNextSiblings = pNextSiblings->ptr2NextSibling;
				}
			}
		}
	}

	else if (relation == "Son") {
		Node* pFirstChild = ptrCurr->ptr2FirstChild;

		if (pFirstChild != NULL) {
			if (pFirstChild->mGender == true)
				queryRes.push_back(pFirstChild->mName);

			Node * pNextSiblings = pFirstChild->ptr2NextSibling;
			while (pNextSiblings != NULL) {
				if (pNextSiblings->mGender == true)
					queryRes.push_back(pNextSiblings->mName);
				pNextSiblings = pNextSiblings->ptr2NextSibling;
			}
		}
	}

	else if (relation == "Daughter") {
		Node* pFirstChild = ptrCurr->ptr2FirstChild;

		if (pFirstChild != NULL) {
			if (pFirstChild->mGender == false)
				queryRes.push_back(pFirstChild->mName);

			Node * pNextSiblings = pFirstChild->ptr2NextSibling;
			while (pNextSiblings != NULL) {
				if (pNextSiblings->mGender == false)
					queryRes.push_back(pNextSiblings->mName);
				pNextSiblings = pNextSiblings->ptr2NextSibling;
			}
		}
	}

	else if (relation == "Siblings") {
		Node* pMother = ptrCurr->ptr2mother;

		if (pMother != NULL) {
			Node* pFirstChild = pMother->ptr2FirstChild;

			if (pFirstChild != NULL) {
				if (pFirstChild != ptrCurr)// exclude self
					queryRes.push_back(pFirstChild->mName);

				Node * pNextSiblings = pFirstChild->ptr2NextSibling;
				while (pNextSiblings != NULL) {
					if (pNextSiblings != ptrCurr)// exclude self
						queryRes.push_back(pNextSiblings->mName);
					pNextSiblings = pNextSiblings->ptr2NextSibling;
				}
			}
		}
	}

	return queryRes;
}


int main() {

	familyTree fTree = familyTree();

	//Building the Basic-Family Tree
	ifstream nameFileout;
	nameFileout.open("FamilyData.txt");
	string line;

	if (!nameFileout) {
		cout << "FamilyData.txt NOT openned properly" << endl;
		return 0;
	}

	while (getline(nameFileout, line)) {
		istringstream ss(line);
		string token;
		vector<string> result;

		if (line == "")
			break;

		while (getline(ss, token, ' '))
			result.push_back(token);

		if (result[0] == "ADD_CHILD") {
			string motherName = result[1];
			string childName = result[2];
			string childGender = result[3];

			fTree.addChild(motherName, childName, childGender == "Male");
		}

		else if (result[0] == "MAKE_COUPLE") {
			string directChild = result[1];
			string partnerName = result[2];

			fTree.makeCouple(directChild, partnerName);
		}
	}
	nameFileout.close();

	// Taking the UserQueries
	nameFileout.open("userData.txt");
	if (!nameFileout) {
		cout << "UserData.txt NOT openned properly" << endl;
		return 0;
	}

	userMode = true;
	while (getline(nameFileout, line)) {
		istringstream ss(line);
		string token;
		vector<string> result;

		if (line == "")
			break;

		while (getline(ss, token, ' '))
			result.push_back(token);

		if (result[0] == "ADD_CHILD") {
			string motherName = result[1];
			string childName = result[2];
			string childGender = result[3];

			fTree.addChild(motherName, childName, childGender == "Male");
		}

		else if (result[0] == "GET_RELATIONSHIP") {
			string personName = result[1];
			string relation = result[2];

			vector<string> output;
			output = fTree.getRelationShip(personName, relation);

			//printing the output on the console
			if (output.size() == 0)
				cout << "NONE" << endl;

			//std::sort(output.begin(), output.end()); // to print in chronological fashion
			for (auto it = output.begin(); it != output.end(); it++)
				cout << *it << " ";
			cout << endl;
		}
	}
	nameFileout.close();

	return 0;
}