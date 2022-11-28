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
    bool isIsLeaf(){
        return this->isLeaf;
    }
    Node* Select(){
        return legalNodes[0];

        Node* bestNode;

        //TODO: return the move idx with max UCT value
        double maxValue = -std::numeric_limits<double>::max();
        double value;
        for(Node* child : legalNodes){
            value = -child->value/(child->visitCount+EPSILON) + sqrt(2*log(visitCount)/(child->visitCount+EPSILON));
            if(value>maxValue){
                maxValue = value;
                bestNode = child;
            }
        }
        return bestNode;
    } 
    void Expand(board::piece_type who){
        //TODO: expand child
        std::vector<action::place> space(board::size_x * board::size_y);
        for (size_t i = 0; i < space.size(); i++)
			space[i] = action::place(i, who);
        //std::cout<<"space size:" << space.size();
        
        for (const action::place& move : space) {
			board after = state;
            std::cout << move.apply(after) << std::endl;
			if (move.apply(after) == board::legal){
                legalNodes.push_back(new Node(after));
                legalMoves.push_back(move);
            }
		}
        //std::cout<<"legalNodes size:" << legalNodes.size();
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

    double Update(double _value){
        //TODO: update value
        visitCount++;
        value += _value;
        return value;
    }
    
    action::place GetBestmove(){
        
        int index = 0;
        double maxValue = -std::numeric_limits<double>::max();
        if(legalMoves.size()==0){
            return action();
        }
        return legalMoves[index];
        for(int i=0; i<legalNodes.size(); i++){
            if(legalNodes[i]->value > maxValue){
                index = i;
                maxValue = legalNodes[i]->value;
            }
        }
    }


private:
    double value = 0;
    int visitCount = 0;
    board state;
    std::vector<action::place> legalMoves;
    std::vector<Node*> legalNodes;
    bool isLeaf = true;
};

class MTCS_Tree{
public:
    MTCS_Tree(board::piece_type _who, board state, int _iter){
        printf("build tree");
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
        printf("start simulate\n");
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
        
        //expand
        currentNode->Expand(who);
        while(!currentNode->isIsLeaf()){
            currentNode = currentNode->Select();
            visitedNode.push(currentNode);
        }
        
        //rollout
        double value = rand();

        //backpropagation
        while(!visitedNode.empty()){
            currentNode = visitedNode.top();
            value = currentNode->Update(value);
        }
        
    }
    
private:
    int iter;
    Node* root;
    std::stack<Node*> visitedNode;
    board::piece_type who;
};