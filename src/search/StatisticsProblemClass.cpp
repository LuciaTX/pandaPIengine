/*
 * StatisticsProblemClass.cpp
 *
 *  Created on: 01.07.2023
 *      Author: Lijia Yuan
 */

#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>   // NOTE

#include "StatisticsProblemClass.h"

namespace progression {

#ifndef DEF_STATISTICS_VARIABLES
#define DEF_STATISTICS_VARIABLES
    int numPrimitive = 0;
    int numAcyclic = 0;
    int numRegular = 0;
    int numTotalOrder = 0;
    int numExploredNode = 0;
    int numTailRecursive = 0;
    map<searchNode*, bool> isNodeTO;
    map<searchNode*, bool> isNodeRegular;
    map<searchNode*, bool> isNodeAcyclic;
    map<searchNode*, bool> isNodeTailRecursive;
#endif
    vector<int> visitedMethod;
    vector<planStep*> tasksContain;

    void findProblemClass(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes) {
        // store all reachable methods
        for (int i = 0; i < n->numContainedTasks; i++) {
            int taskInNode = n->containedTasks[i];
            if (htn->numMethodsForTask[taskInNode] != 0) {
                for (int j = 0; j < htn->numMethodsForTask[taskInNode]; j++) {
                    int method = htn->taskToMethods[taskInNode][j];
                    getAllReachableMethod(htn, method);
                }
            }
        }

        // store all tasks contain in this search node
        if (n->numPrimitive != 0) {
            for (int p = 0; p < n->numPrimitive; p++) {
                planStep *pTask = n->unconstraintPrimitive[p];
                getAllTasks(pTask);
            }
        }
        if (n->numAbstract != 0) {
            for (int p = 0; p < n->numAbstract; p++) {
                planStep *abTask = n->unconstraintAbstract[p];
                getAllTasks(abTask);
            }
        }
        numExploredNode++;

        // check primtive, if ture then it shouble also regular, acyclic and tail-recursive
        if (isNodePrimitive(n, htn)) {
            numPrimitive++;
            numRegular++;
            numAcyclic++;
            numTailRecursive++;
        } else {
            bool tailRecursive = false;
            // if it is regular, then it should be tail-recursive
            if (isRegular(n, htn, fatherNodes)) {
                numRegular++;
                numTailRecursive++;
                tailRecursive = true;
            }
            // if it is acyclic, then it should be tail-recursive
            if (isAcyclic(n, htn, fatherNodes)) {
                numAcyclic++;
                if (!tailRecursive) {
                    numTailRecursive++;
                    tailRecursive = true;
                }
                
            }
            // a task network can be tail-recursive, without being acyclic or regular
            if (!tailRecursive) {
                if (isTailRecursive(n, htn, fatherNodes)) numTailRecursive++;
            }
        }

        if (isTotalOrder(n, htn, fatherNodes)) numTotalOrder++;

        visitedMethod.clear();
        tasksContain.clear();
    }

    /*
        iterate all reachable methods in dfs order
        get all reachable methods
    */
    void getAllReachableMethod(Model *htn, int method) {
        // if this method has not been visited
        if (find(visitedMethod.begin(), visitedMethod.end(), method) == visitedMethod.end()) { 
            // visited this method
            visitedMethod.push_back(method);

            // compound tasks in this method
            for (int ist = 0; ist < htn->numSubTasks[method]; ist++) { 
                int subtask = htn->subTasks[method][ist];
                if (htn->numMethodsForTask[subtask] != 0) {
                    // methods in this compound task
                    for (int m = 0; m < htn->numMethodsForTask[subtask]; m++) {
                        int nextMethod = htn->taskToMethods[subtask][m];
                        getAllReachableMethod(htn, nextMethod);
                    }
                }
            }
        }
        
    }

    /*
        get all reachable tasks from the input task
        and put then into the vector
    */
    void getAllTasks(planStep *task) {
        // if this task is not contained
        if (find(tasksContain.begin(), tasksContain.end(), task) == tasksContain.end()) { 
            // put this task into vector
            tasksContain.push_back(task);
            for (int i = 0; i < task->numSuccessors; i++) {
                planStep *succ = task->successorList[i];
                getAllTasks(succ);
            }
        }
    }

    bool isFatherNodeInProblemClass(searchNode *n, map<searchNode*, searchNode*> fatherNodes, map<searchNode*, bool> inProblemClass) {
        if (fatherNodes.find(n) != fatherNodes.end()) {
            searchNode *fatherN = fatherNodes.find(n)->second;
            if (inProblemClass.find(fatherN) != inProblemClass.end()) {
                // if father node is total order, then n is total order
                if (inProblemClass[fatherN]) {
                    return true;
                }
            }
        }
        return false;
    }

    bool isTotalOrder(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes) {
        // check whether n's father node is total order 
        if (isFatherNodeInProblemClass(n, fatherNodes, isNodeTO)) {
            isNodeTO[n] = true;
            return true;
        }

        // check if all tasks in n are in total order
        if (n->numAbstract == 1 && n->numPrimitive == 0) {
            planStep *AbsTask = n->unconstraintAbstract[0];
            while (AbsTask->numSuccessors != 0) {
                if (AbsTask->numSuccessors > 1) {
                    isNodeTO[n] = false;
                    return false;
                }
                AbsTask = AbsTask->successorList[0];
            }
        }
        else if (n->numPrimitive == 1 && n->numAbstract == 0) {
            planStep *PTask = n->unconstraintPrimitive[0];
            while (PTask->numSuccessors != 0) {
                if (PTask->numSuccessors > 1) {
                    isNodeTO[n] = false;
                    return false;
                }
                PTask = PTask->successorList[0];
            }
        }
        else if (n->numPrimitive == 0 && n->numAbstract == 0) {
            isNodeTO[n] = true;
            return true;
        }
        else {
            isNodeTO[n] = false;
            return false;
        }


        // check whether each method is totally ordered
        for (const auto& method : visitedMethod) {
            if (!htn->isMethodTotallyOrdered(method)) {
                isNodeTO[n] = false;
                return false;
            }
        }
        
        isNodeTO[n] = true;
        return true;
    }

    bool isNodePrimitive(searchNode *n, Model *htn) {
        // iterate all contained tasks
        for (int i = 0; i < n->numContainedTasks; i++) {
            if (!htn->isPrimitive[n->containedTasks[i]]) {
                return false;
            }
        }
        return true;
    }

    bool isRegular(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes) {
        // check whether n's father node is regular
        if (isFatherNodeInProblemClass(n, fatherNodes, isNodeRegular)) {
            isNodeRegular[n] = true;
            return true;
        }

        // no abstract task
        if (isNodePrimitive(n, htn)) {
            isNodeRegular[n] = true;
            return true;
        }
        
        bool oneLastTask = false;
        // iterate all tasks in n
        for (int t = 0; t < tasksContain.size(); t++) {
            int currentT = tasksContain[t]->task;

            // There are two situation that should return false:
            // t is compound task but not the last task
            // t is the last task, but there is other last task
            // if this is not primitive but not last task
            if (!htn->isPrimitive[currentT] ) {
                // not the last task
                if (tasksContain[t]->numSuccessors != 0) {
                    isNodeRegular[n] = false;
                    return false;
                } else { // if this task is the last task
                    // no count last task before
                    if (!oneLastTask) {
                        oneLastTask = true;
                    } else {    // not the only last task (another compound last task)
                        isNodeRegular[n] = false;
                        return false;
                    }
                    
                }
            } else {
                // another primitive last task
                if (tasksContain[t]->numSuccessors == 0) {
                    return false;
                }
            }
        }
        

        // check wether each method is regular
        for (const auto& methodIter : visitedMethod) {
            // more than one last task
            if (htn->numLastTasks[methodIter] > 1) {
                for (int p = 0; p < htn->numSubTasks[methodIter]; p++) {
                    // any task in this method is not primitive
                    if (!htn->isPrimitive[htn->subTasks[methodIter][p]]) {
                        isNodeRegular[n] = false;
                        return false;
                    }
                }
                
            } else {    // only one last task
                int lastTask = htn->methodsLastTasks[methodIter][0];
                // iterate all subtasks in this method
                for (int ist = 0; ist < htn->numSubTasks[methodIter]; ist++) {
                    int subtask = htn->subTasks[methodIter][ist];
                    // if a task is not last task but also not primitive
                    if (!htn->isPrimitive[subtask] && lastTask != subtask) {
                        isNodeRegular[n] = false;
                        return false;
                    }
                }
            }
            
        }

        isNodeRegular[n] = true;
        return true;
    }

    bool isAcyclic(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes) {
        // check whether n's father node is acyclic
        if (isFatherNodeInProblemClass(n, fatherNodes, isNodeAcyclic)) {
            isNodeAcyclic[n] = true;
            return true;
        }

        // 1. check the self loop
        // 2. find all decomposed tasks are in which scc, and check whether any scc has more than one task
        for (const auto& method : visitedMethod) {
            int task = htn->decomposedTask[method];
            // iterate all subtasks in this method
            for (int ist = 0; ist < htn->numSubTasks[method]; ist++) { 
                int subtask = htn->subTasks[method][ist];
                if (subtask == task) {
                    isNodeAcyclic[n] = false;
                    return false;
                }
            }

            // check the scc of this decompsed task
            int scc = htn->taskToSCC[task];
            // if there are more than 1 task in this scc
            if (htn->sccSize[scc] != 1) {
                isNodeAcyclic[n] = false;
                return false;
            }
        }

        isNodeAcyclic[n] = true;
        return true;
    }

    /*
        Check in all reachable method wether the decoposed task's
        last subtask has the same height stratification and
        the other subtasks have lower height stratification
        than the decoposed task
    */
    bool isTailRecursive(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes) {
        // check whether n's father node is tail-recursive
        if (isFatherNodeInProblemClass(n, fatherNodes, isNodeTailRecursive)) {
            isNodeTailRecursive[n] = true;
            return true;
        }

        // rule: once the decompsition happened, the stratum height will not increase
        // iterate all reachable method
        for (const auto& method : visitedMethod) {
            int task = htn->decomposedTask[method];
            // store the original scc
            int orginalSCC = htn->taskToSCC[task];
            // iterate all subtasks in this method
            for (int ist = 0; ist < htn->numSubTasks[method]; ist++) { 
                int subtask = htn->subTasks[method][ist];
                int newSCC = htn->taskToSCC[subtask];
                // check the number of last tasks in the method
                // if there is more than one last task
                if (htn->numLastTasks[method] != 1) {
                    // no subtask should be at same height
                    if (newSCC == orginalSCC) {
                        isNodeTailRecursive[n] = false;
                        return false;
                    }
                // if there is exactly one last task
                } else {
                    // if new scc is the same but decomposed task is not the last task
                    if (subtask != htn->methodsLastTasks[method][0] && newSCC == orginalSCC) {
                        isNodeTailRecursive[n] = false;
                        return false;
                    }  
                }
            }
        }
        
        isNodeTailRecursive[n] = true;
        return true;
    }

    void calStatistics() {
        double statisticsPrimitive = (double) numPrimitive / numExploredNode;
        double statisticsTO = (double) numTotalOrder / numExploredNode;
        double statisticsRegular = (double) numRegular / numExploredNode;
        double statisticsAcyclic = (double) numAcyclic / numExploredNode;
        double statisticsTailRecursive = (double) numTailRecursive / numExploredNode;

        cout << "Total order: " << numTotalOrder << endl;
        cout << "Primitive: " << numPrimitive << endl;
        cout << "Regular: " << numRegular << endl;
        cout << "Acyclic: " << numAcyclic << endl;
        cout << "Tail-recursive: " << numTailRecursive << endl;

        cout << "Explored Nodes: " << numExploredNode << endl;
        
        cout <<  " " << endl;
        cout << "The statistics of the problem class for the explored search space is: " << endl;
        cout << "Total order: " << fixed << setprecision(6) << statisticsTO << endl;
        cout << "Primitive: " << fixed << setprecision(6) << statisticsPrimitive << endl;
        cout << "Regular: " << fixed << setprecision(6) << statisticsRegular << endl;
        cout << "Acyclic: " << fixed << setprecision(6) << statisticsAcyclic << endl;
        cout << "Tail-recursive: " << fixed << setprecision(6) << statisticsTailRecursive << endl;
        cout <<  " " << endl;
    }
}