#include <stack>
#include <tuple>
#include <vector>
#include <random>
#include <algorithm>

#include "action.h"
#include "board.h"

const double EPSILON = 1e-6;

class Node{
public:
    
    Node(){};
    Node(board &b){
        state = b;
    }
    double value = 0;
    int visitCount = 0;
    board state;
    std::vector<action::place> legalMoves;
    std::vector<Node*> legalNodes;
    bool isLeaf = true;

    bool isIsLeaf(){
        return this->isLeaf;
    }
    Node* Select(){
        int index=0;

        //TODO: return the move idx with max UCT value
        double maxValue = -std::numeric_limits<double>::max();
        double value;
        int totalVisit = 0;
        for(int i=0; i<legalNodes.size(); i++){
            totalVisit += legalNodes[i]->visitCount;
        }
        for(int i=0; i<legalNodes.size(); i++){
            value = -legalNodes[i]->value/(legalNodes[i]->visitCount+EPSILON) + sqrt(2*log(totalVisit)/(legalNodes[i]->visitCount+EPSILON));
            if(value>=maxValue){
                maxValue = value;
                index = i;
            }
        }
        return legalNodes[index];
    } 
    void Expand(board::piece_type who){
        //TODO: expand child
        std::vector<action::place> space(board::size_x * board::size_y);
        for (size_t i = 0; i < space.size(); i++)
			space[i] = action::place(i, who);
        
        for (const action::place& move : space) {
			board after = state;
			if (move.apply(after) == board::legal){
                legalNodes.push_back(new Node(after));
                legalMoves.push_back(move);
            }
		}
        if(!legalNodes.empty()){
            isLeaf = false;
        }
    }

    double Rollout(){
        std::vector<action::place> space(board::size_x * board::size_y);
        std::shuffle (space.begin(), space.end(), std::default_random_engine());

        int moves=0;
        board curState = state;
        bool canMove;
        while(true){
            canMove = false;
            for (const action::place& move : space) {
                board tmp = curState;
                if (move.apply(curState) == board::legal){
                    canMove = true;
                    curState = tmp;
                    break;
                }
            }
            if(canMove){
                moves++;
            }
            else{
                break;
            }
        }
        return moves%2==1 ? 1 : 0;
    }

    void Update(double _value){
        //TODO: update value
        visitCount++;
        value += _value;
    }
    
    action::place GetBestmove(){
        
        int index = 0;
        double maxValue = -std::numeric_limits<double>::max();
        if(legalMoves.size()==0){
            return action();
        }
        
        for(int i=0; i<legalNodes.size(); i++){
            //std::cout << legalNodes[i]->value << "  ";
            if(legalNodes[i]->value > maxValue){
                index = i;
                maxValue = legalNodes[i]->value;
            }
        }
        //std::cout << std::endl;
        //std::cout << "select move = " << legalMoves[index] << std::endl;
        return legalMoves[index];
    }


private:
    
    
};

class MTCS_Tree{
public:
    MTCS_Tree(board::piece_type _who, board state, int _iter){
        who = _who;
        iter = _iter;
        root = new Node(state);
    };

    action::place GetBestMove(){
        while(iter>0){
            Simulate();
            iter--;
        }
        //return action();
        return root->GetBestmove();
    }
    
    void Simulate(){
        while(!visitedNode.empty()){
            visitedNode.pop();
        }
        Node* currentNode = root;
        visitedNode.push(currentNode);
        //selection
        while(!currentNode->isIsLeaf()){
            currentNode = currentNode->Select();
            visitedNode.push(currentNode);
        }
        
        double value;
        //expand
        if((visitedNode.size()%2==1 && who==board::black) || (visitedNode.size()%2==0 && who==board::white)){
            currentNode->Expand(board::black);
        }
        else{
            currentNode->Expand(board::black);
        }
        if(!currentNode->isIsLeaf()){
            currentNode = currentNode->Select();
            visitedNode.push(currentNode);
            value = (float) rand()/RAND_MAX;
        }
        
        //rollout
        else{
            value = currentNode->value;
        }
        //value = value>0.5?1:0;

        //backpropagation
        std::cout << "visited nodes: " << visitedNode.size() << std::endl;
        while(!visitedNode.empty()){
            currentNode = visitedNode.top();
            currentNode->Update(value);
            value *= -1;
            visitedNode.pop();
        }
        
        for(int i=0; i<root->legalNodes.size(); i++){
            std::cout << root->legalNodes[i]->value << "  ";
        }
        std::cout << std::endl;
    }
    
private:
    int iter;
    Node* root;
    std::stack<Node*> visitedNode;
    board::piece_type who;
};