/*
 * StatisticsProblemClass.cpp
 *
 *  Created on: 01.07.2023
 *      Author: Lijia Yuan
 */

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
    int numTailAndAcyclic = 0;
    int numAcyclicAndRegular = 0;
    int numTailAndRegular = 0;
    int numOnlyTailRecursive = 0;
    int numProgressedNode = 0;
#endif
    vector<int> visitedMethod;
    vector<planStep*> tasksContain;
    // TODO:
    void findProblemClass(searchNode *n, Model *htn, NodeProperties *node) {
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

        cout << "Reachable methods size: " << visitedMethod.size() << endl;
        
        // if the node is getting by progressed an action away
        // we can inherit some properties from father node
        if (node->isProgressed()) {
            auto fatherNode = node->getFatherNode();
            node->setPrimitive(fatherNode->isPrimitive());
            node->setRegular(fatherNode->isRegular());
            node->setAcyclic(fatherNode->isAcyclic());
            node->setTailRecursive(fatherNode->isTailRecursive());
            if (node->isPrimitive()) {
                numPrimitive++;
            }
            if (node->isRegular()) {
                numRegular++;
            }
            if (node->isAcyclic()) {
                numAcyclic++;
            }
            if (node->isTailRecursive()) {
                numTailRecursive++;
            }
            
            if (isTotalOrder(n, htn, node)) {
                numTotalOrder++;
                node->setTotallyOrdered(true);
            }
            numProgressedNode++;
        } else {
            // check primtive, if ture then it shouble also regular, acyclic and tail-recursive
            if (isNodePrimitive(n, htn)) {
                node->setPrimitive(true);
                node->setRegular(true);
                node->setAcyclic(true);
                node->setTailRecursive(true);
                numPrimitive++;
                numRegular++;
                numAcyclic++;
                numTailRecursive++;
            } else {
                // if it is regular, then it should be tail-recursive
                if (isRegular(n, htn, node)) {
                    numRegular++;
                    numTailRecursive++;
                    node->setTailRecursive(true);
                    node->setRegular(true);
                }
                // if it is acyclic, then it should be tail-recursive
                if (isAcyclic(n, htn, node)) {
                    numAcyclic++;
                    node->setAcyclic(true);
                    if (!node->isTailRecursive()) {
                        numTailRecursive++;
                        node->setTailRecursive(true);
                    }
                }
                // a task network can be tail-recursive, without being acyclic or regular
                if (!node->isTailRecursive()) {
                    cout << "not both, in here" << endl;
                    if (isTailRecursive(n, htn, node)) {
                        cout << "successful + 1" << endl;
                        node->setTailRecursive(true);
                        numTailRecursive++;
                    }
                }
            }

            if (isTotalOrder(n, htn, node)) {
                numTotalOrder++;
                node->setTotallyOrdered(true);
            }
        }

        if (node->isRegular() && node->isAcyclic()) {
            numAcyclicAndRegular++;
        } else if (node->isRegular() && !node->isAcyclic())
        {
            numTailAndRegular++;
        } else if (!node->isRegular() && node->isAcyclic())
        {
            numTailAndAcyclic++;
        } else if (!node->isRegular() && !node->isAcyclic() && node->isTailRecursive())
        {
            numOnlyTailRecursive++;
        }

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

    /*
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
    */
    

    bool isTotalOrder(searchNode *n, Model *htn, NodeProperties *node) {
        // check whether n's father node is total order 
        auto fatherNode = node->getFatherNode();
        if (fatherNode != nullptr && fatherNode->isTotallyOrdered()) {
            node->setTotallyOrdered(true);
            return true;
        }

        // check if all tasks in n are in total order
        if (n->numAbstract == 1 && n->numPrimitive == 0) {
            planStep *AbsTask = n->unconstraintAbstract[0];
            while (AbsTask->numSuccessors != 0) {
                if (AbsTask->numSuccessors > 1) {
                    return false;
                }
                AbsTask = AbsTask->successorList[0];
            }
        }
        else if (n->numPrimitive == 1 && n->numAbstract == 0) {
            planStep *PTask = n->unconstraintPrimitive[0];
            while (PTask->numSuccessors != 0) {
                if (PTask->numSuccessors > 1) {
                    return false;
                }
                PTask = PTask->successorList[0];
            }
        }
        else if (n->numPrimitive == 0 && n->numAbstract == 0) {
            node->setTotallyOrdered(true);
            return true;
        }
        else {
            return false;
        }

        // check whether each method is totally ordered
        for (const auto& method : visitedMethod) {
            if (!htn->isMethodTotallyOrdered(method)) {
                return false;
            }
        }
        
        node->setTotallyOrdered(true);
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

    bool isRegular(searchNode *n, Model *htn, NodeProperties *node) {
        // check whether n's father node is regular
        auto fatherNode = node->getFatherNode();
        if (fatherNode != nullptr && fatherNode->isRegular()) {
            node->setRegular(true);
            return true;
        }

        /*
        // no abstract task
        if (isNodePrimitive(n, htn)) {
            node->setRegular(true);
            return true;
        }
        */

        // 1. Check how many compound task
        // 2. Check if there is a last task exits
        // 3, Check if ther existing last task is the only last task
        int compoundTaskCount = 0;
        int compoundTask = 0;
        int taskWithoutSucc = 0;
        int lastTask = 0;
        // bool lastTaskExist = false;
        for (int t = 0; t < tasksContain.size(); t++) {
            int currentT = tasksContain[t]->task;
            if (!htn->isPrimitive[currentT] ) {
                compoundTaskCount++;
                compoundTask = currentT;
                if (compoundTaskCount > 1) {
                    return false;
                }
            }
            if (tasksContain[t]->numSuccessors == 0) {
                taskWithoutSucc++;
                lastTask = currentT;
            }
        }
        // if last task exists
        if (taskWithoutSucc == 1) {
            // But the only compound task is not the last task
            if (compoundTask != lastTask) {
                return false;
            }
        } else if (taskWithoutSucc != 1 && compoundTaskCount != 0) {
            // not last task but there is a compound task
            return false;
        }

        // check wether each method is regular
        for (const auto& methodIter : visitedMethod) {
            // more than one last task
            if (htn->numLastTasks[methodIter] != 1) {
                for (int p = 0; p < htn->numSubTasks[methodIter]; p++) {
                    // any task in this method is not primitive
                    if (!htn->isPrimitive[htn->subTasks[methodIter][p]]) {
                        return false;
                    }
                }
                
            } else {    // only one last task
                int indexLast = htn->methodsLastTasks[methodIter][0];
                int lastTask = htn->subTasks[methodIter][indexLast];
                // iterate all subtasks in this method
                for (int ist = 0; ist < htn->numSubTasks[methodIter]; ist++) {
                    int subtask = htn->subTasks[methodIter][ist];
                    // if a task is not last task but also not primitive
                    if (!htn->isPrimitive[subtask] && lastTask != subtask) {
                        return false;
                    }
                }
            }
            
        }

        node->setRegular(true);
        return true;
    }

    bool isAcyclic(searchNode *n, Model *htn, NodeProperties *node) {
        // check whether n's father node is acyclic
        auto fatherNode = node->getFatherNode();
        if (fatherNode != nullptr && fatherNode->isAcyclic()) {
            node->setAcyclic(true);
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
                    return false;
                }
            }

            // check the scc of this decompsed task
            int scc = htn->taskToSCC[task];
            // if there are more than 1 task in this scc
            if (htn->sccSize[scc] != 1) {
                return false;
            }
        }

        node->setAcyclic(true);
        return true;
    }

    /*
        Check in all reachable method wether the decoposed task's
        last subtask has the same height stratification and
        the other subtasks have lower height stratification
        than the decoposed task
    */
    bool isTailRecursive(searchNode *n, Model *htn, NodeProperties *node) {
        // check whether n's father node is tail-recursive
        auto fatherNode = node->getFatherNode();
        if (fatherNode != nullptr && fatherNode->isTailRecursive()) {
            node->setTailRecursive(true);
            return true;
        }

        // rule: once the decompsition happened, the stratum height will not increase in the subtask
        // iterate all reachable method
        for (const auto& method : visitedMethod) {
            int task = htn->decomposedTask[method];
            // store the original scc
            int fromSCC = htn->taskToSCC[task];
            // iterate all subtasks in this method
            for (int ist = 0; ist < htn->numSubTasks[method]; ist++) { 
                int subtask = htn->subTasks[method][ist];
                int toSCC = htn->taskToSCC[subtask];
                // check the number of last tasks in the method
                // if there is more than one last task
                if (htn->numLastTasks[method] != 1) {
                    // no subtask should be at same height
                    if (toSCC == fromSCC) {
                        return false;
                    }
                // if there is exactly one last task
                } else {
                    int indexLast = htn->methodsLastTasks[method][0];
                    if (subtask != htn->subTasks[method][indexLast] && toSCC == fromSCC) {
                        return false;
                    }  
                }
            }
        }
        
        node->setTailRecursive(true);
        return true;
    }

    void calStatistics() {
        double statisticsPrimitive = (double) numPrimitive / numExploredNode;
        double statisticsTO = (double) numTotalOrder / numExploredNode;
        double statisticsRegular = (double) numRegular / numExploredNode;
        double statisticsAcyclic = (double) numAcyclic / numExploredNode;
        double statisticsTailRecursive = (double) numTailRecursive / numExploredNode;
        double statisticsTailAndAcyclic = (double) numTailAndAcyclic / numExploredNode;
        double statisticsTailAndRegular = (double) numTailAndRegular / numExploredNode;
        double statisticsOnlyTail = (double) numOnlyTailRecursive / numExploredNode;
        double statisticsAcyReg = (double) numAcyclicAndRegular/ numExploredNode;
        double statisticsProgressed = (double) numProgressedNode/ numExploredNode;


        cout << "Total order: " << numTotalOrder << endl;
        cout << "Primitive: " << numPrimitive << endl;
        cout << "Regular: " << numRegular << endl;
        cout << "Acyclic: " << numAcyclic << endl;
        cout << "Tail-recursive: " << numTailRecursive << endl;
        cout << "Tail-recursive + Acyclic: " << numTailAndAcyclic << endl;
        cout << "Tail-recursive + Regular: " << numTailAndRegular << endl;
        cout << "Both Acyclic and Regular: " << numAcyclicAndRegular << endl;
        cout << "Tail-recursive without Acyclic and Regular: " << numOnlyTailRecursive << endl;
        cout << "Progressed Node: " << numProgressedNode << endl;

        cout << "Explored Nodes: " << numExploredNode << endl;
        
        cout <<  " " << endl;
        cout << "The statistics of the problem class for the explored search space is: " << endl;
        cout << "Total order: " << fixed << setprecision(6) << statisticsTO << endl;
        cout << "Primitive: " << fixed << setprecision(6) << statisticsPrimitive << endl;
        cout << "Regular: " << fixed << setprecision(6) << statisticsRegular << endl;
        cout << "Acyclic: " << fixed << setprecision(6) << statisticsAcyclic << endl;
        cout << "Tail-recursive: " << fixed << setprecision(6) << statisticsTailRecursive << endl;
        cout << "Tail-recursive + Acyclic: " << fixed << setprecision(6) << statisticsTailAndAcyclic << endl;
        cout << "Tail-recursive + Regular: " << fixed << setprecision(6) << statisticsTailAndRegular << endl;
        cout << "Both Acyclic and Regular: " << fixed << setprecision(6) << statisticsAcyReg << endl;
        cout << "Tail-recursive without Acyclic and Regular: " << fixed << setprecision(6) << statisticsOnlyTail << endl;
        cout << "Progressed node: " << fixed << setprecision(6) << statisticsProgressed << endl;
        cout <<  " " << endl;
    }
}