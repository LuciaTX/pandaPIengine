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

<<<<<<< HEAD
    bool isTailRecursive(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes);

=======
>>>>>>> 58a60b2b885244e247519a4bdaa5c5488d20b612
    void calStatistics();

    void getAllReachableMethod(Model *htn, int method);

    void getAllTasks(planStep *task);

    bool isRegular(searchNode *n, Model *htn, map<searchNode*, searchNode*> fatherNodes);

    extern int numPrimitive;

    extern int numAcyclic;

    extern int numRegular;

    extern int numTotalOrder;

    extern int numExploredNode;

<<<<<<< HEAD
    extern int numTailRecursive;
=======
    extern int numPartialOrder;
>>>>>>> 58a60b2b885244e247519a4bdaa5c5488d20b612

    extern map<searchNode*, bool> isNodeTO;

    extern map<searchNode*, bool> isNodeRegular;

    extern map<searchNode*, bool> isNodeAcyclic;

<<<<<<< HEAD
    extern map<searchNode*, bool> isNodeTailRecursive;

=======
>>>>>>> 58a60b2b885244e247519a4bdaa5c5488d20b612
    extern vector<int> visitedMethod;
    
    extern vector<planStep*> tasksContain;
} 
#endif