/** @file
 *****************************************************************************

 Declaration of interfaces for a R1CS example, as well as functions to sample
 R1CS examples with prescribed parameters (according to some distribution).

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef R1CS_EXAMPLES_HPP_
#define R1CS_EXAMPLES_HPP_

#include <libsnark/relations/constraint_satisfaction_problems/r1cs/r1cs.hpp>

namespace libsnark {

/**
 * A R1CS example comprises a R1CS constraint system, R1CS input, and R1CS witness.
 */
template<typename FieldT>
struct r1cs_example {
    r1cs_constraint_system<FieldT> constraint_system;
    r1cs_primary_input<FieldT> primary_input;
    r1cs_auxiliary_input<FieldT> auxiliary_input;

    r1cs_example<FieldT>() = default;
    r1cs_example<FieldT>(const r1cs_example<FieldT> &other) = default;
    r1cs_example<FieldT>(const r1cs_constraint_system<FieldT> &constraint_system,
                         const r1cs_primary_input<FieldT> &primary_input,
                         const r1cs_auxiliary_input<FieldT> &auxiliary_input) :
        constraint_system(constraint_system),
        primary_input(primary_input),
        auxiliary_input(auxiliary_input)
    {};
    r1cs_example<FieldT>(r1cs_constraint_system<FieldT> &&constraint_system,
                         r1cs_primary_input<FieldT> &&primary_input,
                         r1cs_auxiliary_input<FieldT> &&auxiliary_input) :
        constraint_system(std::move(constraint_system)),
        primary_input(std::move(primary_input)),
        auxiliary_input(std::move(auxiliary_input))
    {};
};

template<typename FieldT>
r1cs_example<FieldT> generate_r1cs_example_with_image_convol(const size_t num_kernel_h, size_t num_kernel_w,
				const size_t num_h, const size_t num_w);


template<typename FieldT>
r1cs_example<FieldT> generate_r1cs_example_with_convol(const size_t num_a,
                                                            const size_t num_x);

/**
 * Generate a R1CS example such that:
 * - the number of constraints of the R1CS constraint system is num_constraints;
 * - the number of variables of the R1CS constraint system is (approximately) num_constraints;
 * - the number of inputs of the R1CS constraint system is num_inputs;
 * - the R1CS input consists of ``full'' field elements (typically require the whole log|Field| bits to represent).
 */
template<typename FieldT>
r1cs_example<FieldT> generate_r1cs_example_with_field_input(const size_t num_constraints,
                                                            const size_t num_inputs);

/**
 * Generate a R1CS example such that:
 * - the number of constraints of the R1CS constraint system is num_constraints;
 * - the number of variables of the R1CS constraint system is (approximately) num_constraints;
 * - the number of inputs of the R1CS constraint system is num_inputs;
 * - the R1CS input consists of binary values (as opposed to ``full'' field elements).
 */
template<typename FieldT>
r1cs_example<FieldT> generate_r1cs_example_with_binary_input(const size_t num_constraints,
                                                             const size_t num_inputs);


///////NEW SYSTEM/////

/*
template<typename FieldT>
struct r1cs_convol_example {
    r1cs_constraint_convol_system<FieldT> constraint_system;
    r1cs_primary_input<FieldT> primary_input;
    r1cs_auxiliary_input<FieldT> auxiliary_input;

    r1cs_convol_example<FieldT>() = default;
    r1cs_convol_example<FieldT>(const r1cs_convol_example<FieldT> &other) = default;
    r1cs_convol_example<FieldT>(const r1cs_constraint_convol_system<FieldT> &constraint_system,
                         const r1cs_primary_input<FieldT> &primary_input,
                         const r1cs_auxiliary_input<FieldT> &auxiliary_input) :
        constraint_system(constraint_system),
        primary_input(primary_input),
        auxiliary_input(auxiliary_input)
    {};
    r1cs_convol_example<FieldT>(r1cs_constraint_convol_system<FieldT> &&constraint_system,
                         r1cs_primary_input<FieldT> &&primary_input,
                         r1cs_auxiliary_input<FieldT> &&auxiliary_input) :
        constraint_system(std::move(constraint_system)),
        primary_input(std::move(primary_input)),
        auxiliary_input(std::move(auxiliary_input))
    {};
};
*/
template<typename FieldT>
		r1cs_example<FieldT> generate_r1cs_origin_convol_example(const size_t num_inputs, const std::vector<FieldT> inputs, const size_t num_kernels, const std::vector<FieldT> kernels);

/*
template<typename FieldT>
		r1cs_convol_example<FieldT> generate_r1cs_convol_example(const size_t num_inputs, const std::vector<FieldT> inputs, const size_t num_kernels, const std::vector<FieldT> kernels, const size_t num_convol);

template<typename FieldT>
		r1cs_convol_example<FieldT> generate_r1cs_example_with_field_input(r1cs_constraint_system<FieldT> cs, const size_t num_constraints,
				const size_t num_inputs);
template<typename FieldT>
         r1cs_convol_example<FieldT> generate_r1cs_convol_combi_example(const size_t num_inputs, const std::vector<FieldT> inputs, const size_t num_kernels, const std::vector<FieldT> kernels, const size_t num_convol, const size_t num_input2, const size_t num_const);
*/
} // libsnark


#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.tcc>

#endif // R1CS_EXAMPLES_HPP_
