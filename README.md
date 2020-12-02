# RDF-Triple-adapter
The publication of machine-readable information has been significantly increasing
both in the magnitude and complexity of the embedded relations. The Resource
Description Framework(RDF) plays a big role in modeling and linking web data and
their relations. In line with that important role, dedicated systems were designed
to store and query the RDF data using a special queering language called SPARQL
similar to the classic SQL. However, due to the high size of the data, several federated
working nodes were used to host a distributed RDF store. The data needs to be
partitioned, assigned, and stored in each working node. After partitioning, some of
the data needs to be replicated in order to avoid the communication cost, and balance
the loads for better system throughput. Since replications require more storage space,
the important two questions are: what data to replicate? And how much? The
answer to the second question is related to other storage-space requirements at each
working node like indexes and cache. In order to efficiently answer SPARQL queries,
each working node needs to put its share of data into multiple indexes. Those indexes
have a data-wide size and consume a considerable amount of storage space. In this
context, the same two questions about replications are also raised about indexes.
The third storage-consuming structure is the join cache. It is a special index where
the frequent join results are cached and save a considerable amount of running time
on the cost of high storage space consumption. Again, the same two questions of
replication and indexes are applicable to the join-cache.
In this thesis, we present a universal adaption approach to the storage of a distributed
RDF store. The system aims to find optimal data assignments to the different
indexes, replications, and join cache within the limited storage space. To achieve
this, we present a cost model based on the workload that often contains frequent
patterns. The workload is dynamically analyzed to evaluate predefined rules. Those
rules tell the system about the benefits and costs of assigning which data to what
structure. The objective is to have better query execution time.
Besides the storage adaption, the system adapts its processing resources with the
queries’ arrival rate. The aim of this adaption is to have better parallelization per
query while still provides high system throughput.
