#ifndef CHRONISTA_OPERATION
#define CHRONISTA_OPERATION

namespace chronista
{
    enum OperationType : unsigned int
    {
        Read,
        Write,
        Commit
    };
    enum Granularity : unsigned int
    {
        DB,
        File,
        Table,
        Tuple,
        Index,
        None
    };

    class Operation
    {
    private:
        OperationType operation_type;
        int transaction_id;
        Granularity granularity;
        /// @brief Parse the operation string
        /// @param operation String to be parsed. Example: "T1: r(a.b.c)"
        std::tuple<OperationType, int, Granularity> parse_operation(std::string operation);

    public:
        Operation(const std::string operation);
        /// @brief Getter for operation_type
        OperationType get_operation();
        /// @brief Getter for transaction_id
        int get_transaction_id();
        /// @brief Getter for granularity
        Granularity get_granularity();
    };
} // namespace chronista

#endif