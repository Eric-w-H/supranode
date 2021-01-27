**SupraNode**
=============
README is WIP.
An extensible computational graph library.

**About**
---------
SupraNode describes a directed computational graph with edges of data and nodes of computations. There is always one source and one sink, though the internal graph may have as many branches as necessary. The name comes from the nature of SupraNode graphs: everything is a Node! To ease construction of more complicated graphs, helper functions are included -- but they always boil down to encapsulating Nodes in more Nodes. Because SupraNode is a header-only library, it can be dropped straight into your project!

### **Dependencies**
[Boost::Hana](https://www.boost.org/doc/libs/1_63_0/libs/hana/doc/html/index.html).
Also tested with [Eigen3](https://eigen.tuxfamily.org/dox/).

### **Nodes**
Nodes are the core structure in SupraNode. A Node is defined by the parameters it takes, and can change the functions it implements at runtime. Each Node has four function "slots", known as Connections, and labelled by their parameter types: input->output, output->input, input->input, and output->output. A Connection takes const references to (or temporary copies of) the first set of parameters, and updates pointers to the second set of parameters. Each Node owns one of each parameter on the heap, i.e. a tuple of input (pointer)s and a tuple of output (pointer)s. Nodes return references to the data when queried for raw_updates or raw_outputs. An 'update' is a tuple of input(s) _leaving_ the current Node. In a graph, or simple sequence of Nodes, 

### **Networks**
Networks are Nodes that contain Nodes -- simple as that. Each Network represents a source and a sink, and can be used to train the neural network. Networks must be generated via the overloaded `>>` operator from an Input. Networks consist of an Input followed by one or more Layers. They may be terminated with an Output.