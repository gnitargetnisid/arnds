# arnds
ARNDS [Automatic Reasoning Natural Deduction Solver] is an interactive natural deduction proof assistant for first order logic. This implementation is done for the purposes of the student project on the subject of Automatic Reasoning for the Faculty of Mathematics at University of Belgrade.


## Description
ARNDS uses a natural deduction tree which contains premises and formulas. This tree is modified by the user either by adding new premises or deriving previous ones into formulas which extend the tree further. ARNDS supports automatic removal/elimination of all viable premises when the appropriate rules are applied.

## How to run
ARNDS uses cmake as its solution/project generator. 
To build and run on Linux:
```bash
cd arnds/code/
mkdir build
cmake -S "" -B "build"
cd build
make
cd src/bin
./arnds
```

## Functionality
* Add premises to the natural deduction tree using functional-style syntax to describe the formulas in these premises.
* Duplicate/Instantiate existing premises by providing the ID of the premise. This helps when there's multiple instances of the same premise that need to be removed.
* Apply one of the natural deduction rules on the particular nodes in the natural deduction tree. Any premises that shall be eliminated will be eliminated automatically.
* Print the status of the particular node or a premise by providing their ID. The status contains the current state of the node ([Pending/Resolved] for inner-node formulas, [Pending/Eliminated] for premises) and the child node derived from the formula/premise in case they're resolved.
* List all current tree nodes and their statuses. List all current premises and their statuses. List only pending nodes. List only pending premises and their pending instances.

## Limitations
* Natural deduction rules which rely on semantic equivalence between formulas will fail to be parsed. This is because ARNDS currently doesn't support function terms and valuations, and as such, it's impossible to evaluate all the truth values of all formulas. This might be supported in the future. In the meantime, make sure any formulas passed to the rules that rely on semantic equivalence are syntactically equivalent.
* ARNDS doesn't differentiate between uppercase and lowercase letters at the moment. This means that all of the inputs you provide will be converted and printed in lowercase letters. This should be handled soon.

## How to use
Below is the list of commands and the parameters of these commands which you can use to communicate with ARNDS. Each command should be specified in one line of the input, and its arguments should be separated by '|'. Spaces don't matter as the parser strips them anyway. Same goes for lowercase and uppercase letters.

### Commands
```
Add | [premise]
```
Add a new *[premise]*, instantiate it, and add the instance to the natural deduction tree. The ID of the premise and its instance will be printed to the standard output. *[premise]* is a formula, the syntax of which is specified later in the document.


``` 
Print | [what] | [ID]
```
Prints the status of the premise/formula with the following *[ID]*. *[what]* should be one of {'P' -- premise ; 'F' -- formula}. Printed information consists of the status of the node (or instance nodes if you want to print the premise state) currently -- this includes the current state (Resolved/Pending) and in case the node is resolved, the ID of the child node.


``` 
Duplicate | [ID]
```
Duplicate the premise with the following *[ID]*. This adds another instance of the premise to the natural deduction tree and prints the ID of the instance.


``` 
List | [what] 
```
Lists current premises or formulas. *[what]* should be one of {'P' - All premises; 'PP' - Pending premises; 'F' - All formulas; 'FP' - Pending formulas }


``` 
Apply | [rule] | [rule_args] ... 
```
Apply the natural deduction rule *[rule]* with *[rule_args]*. Below is the list of rules and arguments that they expect.

### Natural deduction Rules
Formula nodes and premises are specified by their IDs. In case there's multiple instances of the same premise that can be eliminated with the use of the rule, they will all be eliminated.

``` 
[IN] | [contradiction_node] | [premise]
```
- Not Introduction: 
Expects the formula of *[contradiction_node]* to be 'False' and derives the negation of *[premise]*. Expects at least a single instance of *[premise]* to exist in the deduction tree of *[contradiction_node]*. Eliminates all instances of *[premise]* in the deduction tree of *[contradiction_node]*

```
[EN] | [formula_node] | [formula_negation_node] 
```
- Not Elimination:
Expects the formula of *[formula_negation_node]* to be a syntactical negation of the formula of *[formula_node]*. Derives 'False'.

``` 
[IC] | [lhs_node] | [rhs_node] 
```
- Conjunction Introduction:
Derives conjunction of the formulas of *[lhs_node]* and *[rhs_node]*.

```
[ELC] | [conjunction_node] 
```
- Conjunction Elimination LHS:
Expects the formula of *[conjunction_node]* to be a conjunction. Derives the left hand side of the formula of *[conjunction_node]*.

```
[ERC] | [conjunction_node] 
```
- Conjunction Elimination RHS:
Expects the formula of *[conjunction_node]* to be a conjunction. Derives the right hand side of the formula of *[conjunction_node]*.

``` 
[ILD] | [lhs_formula_node] | [rhs_formula] 
``` 
- Disjunction Introduction LHS: 
Derives disjunction of the formula of *[lhs_formula_node]* and *[rhs_formula]*.

```
[IRD] | [rhs_formula_node] | [lhs_formula] 
```
- Disjunction Introduction RHS:
Derives disjunction of the formula of *[lhs_formula]* and *[rhs_formula_node]*.

```
[ED] | [disjunction_node] | [lhs_node] | [rhs_node] | [lhs_premise] | [rhs_premise] 
```
- Disjunction Elimination:
Expects formula of *[lhs_node]* and the formula of *[rhs_node]* to be syntactically equivalent. Expects the formula of *[disjunction_node]* to be a disjunction of the formulas of *[lhs_premise]* and *[rhs_premise]*, in that order. Expects that at least a single instance of *[lhs_premise]* exists in the deduction tree of *[lhs_node]*. Expects that at least a single instance of *[rhs_premise]* exists in the deduction tree of *[rhs_node]*.  nalazi u stablu izvođenja od *[rhs_node]*. Derives the formula of *[lhs_node]/[rhs_node]*. Eliminates all instances of the premise *[lhs_premise]* in the deduction tree of *[lhs_node]*. Eliminates all instances of the premise *[rhs_premise]* in the deduction tree of *[rhs_node]*.

```
[II] | [formula_node] | [premise] 
```
- Implication Introduction:
Derives the implication of the formula of *[formula_node]* with the formula of *[premise]*. Expects that at least a single instance of *[premise]* exists in the deduction tree of *[formula_node]*. Eliminates all instances of *[premise]* in the deduction tree of *[formula_node]*.

```
[EI] | [implier_node] | [implication_node] 
```
- Implication Elimination:
Expects the formula of *[implication_node]* to be an implication. Expects the formula of *[implier_node]* to be the left hand side of the implication formula in *[implication_node]*. Derives the right hand side of the formula in *[implication_node]*.

```
[EC] | [contradiction_node] | [formula] 
```
- Contradiction Elimination:
Expects the formula of *[contradiction_node]* to be a 'False'. Derives *[formula]*.

```
[IT] | 
```
- True Introduction
Derives 'True'.

```
[IF] | [formula_node] [variable_x] [variable_y]
```
- Forall Introduction

Expects the variable *[variable_x]* to be a free variable in the formula of *[formula_node]*. Expects *[variable_x]* to not be present as a free variable in any of the unresolved premises in the deduction tree of *[formula_node]*. Derives the Forall formula whose bound variable is *[variable_x]* and internal formula is the formula of *[formula_node]*. Changes all instances of *[variable_x]* with *[variable_y]* in the derived formula.

```
[EF] || [forall_node] [variable]
```
- Forall Elimination
Expects the formula of *[forall_node]* to be a Forall formula. Derives the internal formula of the formula of *[forall_node]* where every instance of the bound variable is replaced with *[variable]*.

```
[IE] || [formula_node] [variable_x] [variable_y]
```
- Exists Elimination
Expects the variable *[variable_x]* to be free in the formula of *[formula_node]*. Derives an Exists formula with the bound variable *[variable_x]* and with the internal formula as the formula of *[formula_node]*. Replaces all instances of variable *[variable_x]* with variable *[variable_y]* in the derived formula.

```
[EE] || [exists_node] [conclusion_node] [premise] [variable]
```
- Exists Elimination
Expects the formula of *[forall_node]* to be an Exists formula. Expects the formula of *[premise]* to be syntactically equivalent to the internal formula of the formula of *[exists_node]* where all instances of the bound variable are replaced with the variable *[variable]*. Expects the variable *[variable]* to not be free in the formula of *[conclusion_node]*, as well as in any premises in the deduction tree of *[conclusion_node]*. Expects that at least a single instance of *[premise]* exists in the deduction tree of *[conclusion_node]*. Derives the internal formula of the formula in *[еxists_node]* where every instance of the bound variable is replaced with *[variable]*. Eliminates all instances of the premise *[premise]* which exist in the deduction tree of *[conclusion_node]*.

### Formula syntax
To parse formulas, function-like syntax is used. For example, Exists(x, And(Or(Not(R), P(x)), Imp(R, P(y)))) is an example of one formula.
Namely, supported "functions" are: Not, Or, And, Imp, Forall, Exists. Atoms are expressed using function-like syntax as well, for example R(x, y, z) is an atom. Atoms can have arbitrary arguments, but if they're denoted with the same symbol, they must have the same arity everywhere.
