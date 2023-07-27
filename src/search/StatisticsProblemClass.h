/*
 * StatisticsProblemClass.h
 *
 *  Created on: 01.07.2023
 *      Author: Lijia Yuan
 */

#include <map>
#include <vector>

#ifndef STATISTICS_PROBLEM_CLASS_H
#define STATISTICS_PROBLEM_CLASS_H

#include "../Model.h"

namespace progression {

    void findProblemClass(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes);

    bool isTotalOrder(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes);

    bool isNodePrimitive(searchNode *n, Model *htn);

    bool isRegular(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes);

    bool isAcyclic(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes);

    bool isTailRecursive(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes);

    void calStatistics();

    void getAllReachableMethod(Model *htn, int method);

    void getAllTasks(planStep *task);

    bool isRegular(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes);

    extern int numPrimitive;

    extern int numAcyclic;

    extern int numRegular;

    extern int numTotalOrder;

    extern int numExploredNode;

    extern int numTailRecursive;

    extern map<searchNode*, bool> isNodeTO;

    extern map<searchNode*, bool> isNodeRegular;

    extern map<searchNode*, bool> isNodeAcyclic;

    extern map<searchNode*, bool> isNodeTailRecursive;

    extern vector<int> visitedMethod;
    
    extern vector<planStep*> tasksContain;
} 
#endif