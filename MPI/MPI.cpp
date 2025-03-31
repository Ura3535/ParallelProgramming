#include <mpi.h>
#include <iostream>
#include <vector>
#include <chrono>
#include "Shared.h"

namespace ch = std::chrono;

template<typename Pred>
std::vector<int> MPI_take_where(const std::vector<int>& data, Pred pred, MPI_Datatype mpi_datatype, int root, MPI_Comm comm) {
    int rank, size, data_size = data.size();
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

	MPI_Bcast(&data_size, 1, MPI_INT, root, comm);

    std::vector<int> send_counts(size, data_size / size);
    std::vector<int> displs(size, 0);
    for (int i = 0; i < data_size % size; ++i)
        ++send_counts[i];
    for (int i = 1; i < size; ++i)
        displs[i] = displs[i - 1] + send_counts[i - 1];
    std::vector<int> local_data(send_counts[rank]);

    MPI_Scatterv(data.data(), send_counts.data(), displs.data(), mpi_datatype,
        local_data.data(), send_counts[rank], mpi_datatype,
        root, comm);

    std::vector<int> local_filtered_data = ParallelProgShared::take_where(local_data, pred);
    const int local_count = local_filtered_data.size();
    std::vector<int> recv_counts(size);

    MPI_Gather(&local_count, 1, MPI_INT,
        recv_counts.data(), 1, MPI_INT,
        root, comm);

    std::vector<int> global_filtered;
    std::vector<int> displs2(size, 0);

    if (rank == 0) {
        int total_size = 0;
        for (int i = 0; i < size; i++) {
            displs2[i] = total_size;
            total_size += recv_counts[i];
        }
        global_filtered.resize(total_size);
    }

    MPI_Gatherv(local_filtered_data.data(), local_count, mpi_datatype,
        global_filtered.data(), recv_counts.data(), displs2.data(), mpi_datatype,
        root, comm);

    return global_filtered;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n = 10'000'000;
    std::vector<int> data;

    if (rank == 0) {
		data.resize(n);
        for (int i = 0; i < n; i++) {
            data[i] = i;
        }
    }

    auto pred = ParallelProgShared::is_prime;
    std::vector<int> res;

    auto duration = ParallelProgShared::measure_time(
        [&]() { res = MPI_take_where(data, pred, MPI_INT, 0, MPI_COMM_WORLD); }
    );

    MPI_Finalize();

    if (rank == 0) {
        std::cout << duration.count() / 1000.0 << ' ';
    }
    return 0;
}
