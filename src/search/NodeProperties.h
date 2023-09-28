#ifndef NODE_PROPERTIES_H
#define NODE_PROPERTIES_H

class NodeProperties {
private:
    NodeProperties* fatherNode;
    bool primitive;
    bool totallyOrdered;
    bool regular;
    bool acyclic;
    bool tailRecursive;
    int childrenNodeLeft;
    bool progressedAction;

public:
    NodeProperties(NodeProperties* father = nullptr, bool prim = false, bool order = false, bool reg = false, bool acyc = false, bool tailRec = false, int children = 0, bool progressed = false)
    : fatherNode(father), primitive(prim), totallyOrdered(order), regular(reg), acyclic(acyc), tailRecursive(tailRec), childrenNodeLeft(children), progressedAction(progressed) {}

    bool isPrimitive() const {
        return primitive;
    }

    void setPrimitive(bool value) {
        primitive = value;
    }

    bool isTotallyOrdered() const {
        return totallyOrdered;
    }

    void setTotallyOrdered(bool value) {
        totallyOrdered = value;
    }

    bool isRegular() const {
        return regular;
    }

    void setRegular(bool value) {
        regular = value;
    }

    bool isAcyclic() const {
        return acyclic;
    }

    void setAcyclic(bool value) {
        acyclic = value;
    }

    bool isTailRecursive() const {
        return tailRecursive;
    }

    void setTailRecursive(bool value) {
        tailRecursive = value;
    }
    
    NodeProperties* getFatherNode() const {
        return fatherNode;
    }

    void setFatherNode(NodeProperties* father) {
        fatherNode = father;
    }

    int getChildrenNodeLeft() const {
        return childrenNodeLeft;
    }

    void deleteOneChildrenNode() {
        childrenNodeLeft--;
    }

    void addOneChildrenNode() {
        childrenNodeLeft++;
    }

    bool isProgressed() const {
        return progressedAction;
    }
 
    void setProgressed(bool value) {
        progressedAction = value;
    }
};

#endif