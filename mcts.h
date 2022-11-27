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
        action::place bestmove;
        Node* bestNode;
        //TODO: return the move idx with max UCT value
        double maxValue = -std::numeric_limits<double>::max();
        double value;
        for(std::tuple<action::place, Node*> child : legalNodes){
            value = std::get<1>(child)->value/(std::get<1>(child)->visitCount+EPSILON) + sqrt(log(visitCount)/(std::get<1>(child)->visitCount+EPSILON));
            if(value>maxValue){
                maxValue = value;
                bestNode = std::get<1>(child);
            }
        }
        return bestNode;
    } 
    void Expand(){
        //TODO: expand child
        std::vector<action::place> space(board::size_x * board::size_y);
        for (const action::place& move : space) {
			board after = state;
			if (move.apply(after) == board::legal){
                legalNodes.push_back(std::tuple{move, new Node(after)} );
            }
		}
        isLeaf = false;
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

    }


private:
    double value = 0;
    int visitCount = 0;
    board state;
    int bestMoveIdx;
    std::vector<std::tuple<action::place, Node*> > legalNodes;
    bool isLeaf = true;
};

class MTCS_Tree{
public:
    MTCS_Tree(board state, int _iter){
        iter = _iter;
        root = new Node(state);
    };

    action::place GetBestMove(){
        while(iter>0){
            Simulate();
            iter--;
        }
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

        //expand
        currentNode->Expand();
        currentNode = currentNode->Select();
        visitedNode.push(currentNode);

        //rollout
        double value = currentNode->Rollout();

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
};