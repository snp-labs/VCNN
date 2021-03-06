/** @file
 *****************************************************************************

 Implementation of functions to sample R1CS examples with prescribed parameters
 (according to some distribution).

 See mr1cs_examples.hpp .

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef MR1CS_EXAMPLES_TCC_
#define MR1CS_EXAMPLES_TCC_

#include <cassert>

#include <libff/common/utils.hpp>

namespace libsnark {

	template<typename FieldT>
		mr1cs_example<FieldT> generate_mr1cs_example_with_image_convol(const size_t num_kernel,
				const size_t num_w, const size_t num_h)
		{
			libff::enter_block("Call to generate_mr1cs_example_with_image_convol");

			mr1cs_constraint_system<FieldT> cs;
			cs.primary_input_size = (num_kernel)*(num_kernel)+(num_w)*(num_h);
			cs.auxiliary_input_size = (num_kernel+num_w-1)*(num_kernel+num_h-1); // TODO: explain this
			size_t num_constraints = cs.auxiliary_input_size;

			libff::enter_block("set variables");
			///TODO need to change
			mr1cs_variable_assignment<FieldT> full_variable_assignment;
			size_t sumk=0;
			for(size_t i=0; i<(num_kernel)*(num_kernel); i++){
				full_variable_assignment.push_back(i+1);
				sumk+=(i+1);
			}
			size_t sumx = 0;
			for(size_t i=0; i<(num_w)*(num_h); i++){
				full_variable_assignment.push_back(i+1);
				sumx+=(i+1);
			}
			libff::leave_block("set variables");
			libff::enter_block("set constraints");
			for (size_t i = 0; i < num_constraints-1; ++i)
			{
					std::cout<<"i : "<<i<<std::endl;
				FieldT a = FieldT::zero();
				FieldT x = FieldT::zero();
				FieldT s = FieldT(i+1);
				FieldT t = FieldT(i+1);
				linear_combination<FieldT> A, B, C;

				// (a00 * s^0*t^0 + a01 * s^0*t^1 + a11 * s^1*t^1 + ... )(x00 * s^0*t^0 + x01*s^0*t^1+...)=(y00*s^0*t^0+y01*s^0*t^1+....)
				//std::cout<<"s : "<<s.as_ulong()<<", t : "<<t.as_ulong()<<std::endl;
				FieldT temp_s = FieldT::one();
				FieldT temp_t = FieldT::one();
				for(size_t j=0; j<(num_kernel);j++)
				{
					for(size_t k=0; k<(num_kernel);k++){
						A.add_term((j*num_kernel)+k+1, temp_s*temp_t);
						//std::cout<<"ker : "<<(j*num_kernel)+k+1<<" : "<<((s^j)*(t^k)).as_ulong()<<std::endl;
						temp_t *= t;
					}
					temp_s *= s;
				}
				temp_s = FieldT::one();
				temp_t = FieldT::one();
				//std::cout<<"s : "<<s.as_ulong()<<", t : "<<t.as_ulong()<<std::endl;
				for(size_t j=0; j<num_w;j++)
				{
					for(size_t k=0; k<(num_h);k++){
						B.add_term((num_kernel*num_kernel)+(num_w*j)+k+1, temp_s*temp_t); 
						//std::cout<<"im : "<<((num_kernel*num_kernel)+j*num_w)+k+1<<" : " <<((s^j)*(t^k)).as_ulong()<<std::endl;
						temp_t *= t;
					}
					temp_s *= s;
				}
				temp_s = FieldT::one();
				temp_t = FieldT::one();
				for(size_t j=0; j<(num_w+num_kernel-1);j++){
					for(size_t k=0; k<(num_h+num_kernel-1);k++){
						C.add_term((num_kernel*num_kernel)+(num_w*num_h)+(num_w+num_kernel-1)*j+k+1, temp_s*temp_t);
						temp_t *= t;
					}
					temp_s *= s;
				}

				cs.add_constraint(mr1cs_constraint<FieldT>(A, B, C));
			}
			libff::leave_block("set constraints");
			size_t sumy=0;
			size_t ycount = 0;
			//std::cout<<"y : ";
			libff::enter_block("Compute y variables");
			for(size_t j=0; j<(num_w+num_kernel-1);j++){
				for(size_t k=0; k<(num_h+num_kernel-1);k++){
					FieldT y = FieldT::zero();
					for(size_t w=0; w<num_w;w++){
						for(size_t h=0;h<num_h;h++){
							for(size_t k1=0; k1<num_kernel;k1++){
								for(size_t k2=0; k2<num_kernel;k2++){
									if((w+k1) == j && (h+k2) == k)
									{
										//std::cout<<"w,h,k1,k2 = "<<w<<h<<k1<<k2<<std::endl;
										y += FieldT(k1*num_kernel+k2+1)*FieldT(w*num_w+h+1);
									}
								}
							}
						}
					}
					//std::cout<<y.as_ulong()<< " ";
					ycount++;
					sumy +=y.as_ulong();
					full_variable_assignment.push_back(y);
				}
			}
			libff::leave_block("Compute y variables");
			//std::cout<<std::endl;
			std::cout<<"ycount = "<<ycount<<std::endl;
			std::cout<<"sumx = "<<sumx<<" sumk = "<<sumk<<" suny = "<<sumy<<std::endl;

			/* split variable assignment */
			mr1cs_primary_input<FieldT> primary_input(full_variable_assignment.begin(), full_variable_assignment.begin() + cs.primary_input_size);
			mr1cs_primary_input<FieldT> auxiliary_input(full_variable_assignment.begin() + cs.primary_input_size, full_variable_assignment.end());

			/* sanity checks */
			assert(cs.num_variables() == full_variable_assignment.size());
			assert(cs.num_variables() >= cs.primary_input_size);
			assert(cs.num_inputs() == cs.primary_input_size);
			assert(cs.num_constraints() == num_constraints);
			assert(cs.is_satisfied(primary_input, auxiliary_input));

			libff::leave_block("Call to generate_mr1cs_example_with_image_convol");

			return mr1cs_example<FieldT>(std::move(cs), std::move(primary_input), std::move(auxiliary_input));
		}

	template<typename FieldT>
		mr1cs_example<FieldT> generate_mr1cs_example_with_convol(const size_t num_a,
				const size_t num_x)
		{
			libff::enter_block("Call to generate_mr1cs_example_with_convol");

			mr1cs_constraint_system<FieldT> cs;
			cs.primary_input_size = num_a+num_x;
			cs.auxiliary_input_size = num_a+num_x-1; // TODO: explain this
			size_t num_constraints = 2*num_a+2*num_x-1;

			mr1cs_variable_assignment<FieldT> full_variable_assignment;
			for(size_t i=0; i<num_a; i++){
				full_variable_assignment.push_back(i+1);
			}
			for(size_t i=0; i<num_x; i++){
				full_variable_assignment.push_back(i+1);
			}

			for (size_t i = 0; i < num_constraints-1; ++i)
			{
				FieldT a = FieldT::zero();
				FieldT x = FieldT::zero();
				FieldT z = FieldT(i+1);
				linear_combination<FieldT> A, B, C;

				// (a0 * z^0 + a1 * z^1 + a2 * z^2 + ... )(x0 * z^0 + x1*z^1+...)=(y0*z^0+y1*z^1+....)
				for(size_t j=0; j<num_a;j++)
				{
					A.add_term(j+1, (z^j));
				}
				for(size_t j=0; j<num_x;j++)
				{
					B.add_term(num_a+j+1, (z^j)); 
				}
				for(size_t j=0; j<num_a+num_x-1;j++){
					C.add_term(num_a+num_x+j+1, (z^j));
					FieldT y = FieldT::zero();
					for(size_t k=0; k<num_a;k++){
						for(size_t l=0;l<num_x;l++){
							if((k+l) == j)
							{
								y += FieldT(k+1)*FieldT(l+1);
							}
						}
					}
					full_variable_assignment.push_back(y);
				}

				cs.add_constraint(mr1cs_constraint<FieldT>(A, B, C));
			}

			size_t num_input = num_a+num_x;
			/* split variable assignment */
			mr1cs_primary_input<FieldT> primary_input(full_variable_assignment.begin(), full_variable_assignment.begin() + num_a+num_x);
			mr1cs_primary_input<FieldT> auxiliary_input(full_variable_assignment.begin() + num_a+num_x, full_variable_assignment.end());

			/* sanity checks */
			assert(cs.num_variables() == full_variable_assignment.size());
			assert(cs.num_variables() >= num_a+num_x);
			assert(cs.num_inputs() == num_inputs);
			assert(cs.num_constraints() == num_constraints);
			assert(cs.is_satisfied(primary_input, auxiliary_input));

			libff::leave_block("Call to generate_mr1cs_example_with_convol");

			return mr1cs_example<FieldT>(std::move(cs), std::move(primary_input), std::move(auxiliary_input));
		}

	template<typename FieldT>
		mr1cs_example<FieldT> generate_mr1cs_example_with_field_input(const size_t num_constraints,
				const size_t num_inputs)
		{
			libff::enter_block("Call to generate_mr1cs_example_with_field_input");

			assert(num_inputs <= num_constraints + 2);

			mr1cs_constraint_system<FieldT> cs;
			cs.primary_input_size = num_inputs;
			cs.auxiliary_input_size = 2 + num_constraints - num_inputs; // TODO: explain this

			mr1cs_variable_assignment<FieldT> full_variable_assignment;
			FieldT a = FieldT::random_element();
			FieldT b = FieldT::random_element();
			full_variable_assignment.push_back(a);
			full_variable_assignment.push_back(b);

			for (size_t i = 0; i < num_constraints-1; ++i)
			{
				linear_combination<FieldT> A, B, C;

				if (i % 2)
				{
					// a * b = c
					A.add_term(i+1, 1);
					B.add_term(i+2, 1);
					C.add_term(i+3, 1);
					FieldT tmp = a*b;
					full_variable_assignment.push_back(tmp);
					a = b; b = tmp;
				}
				else
				{
					// a + b = c
					B.add_term(0, 1);
					A.add_term(i+1, 1);
					A.add_term(i+2, 1);
					C.add_term(i+3, 1);
					FieldT tmp = a+b;
					full_variable_assignment.push_back(tmp);
					a = b; b = tmp;
				}

				cs.add_constraint(mr1cs_constraint<FieldT>(A, B, C));
			}

			linear_combination<FieldT> A, B, C;
			FieldT fin = FieldT::zero();
			for (size_t i = 1; i < cs.num_variables(); ++i)
			{
				A.add_term(i, 1);
				B.add_term(i, 1);
				fin = fin + full_variable_assignment[i-1];
			}
			C.add_term(cs.num_variables(), 1);
			cs.add_constraint(mr1cs_constraint<FieldT>(A, B, C));
			full_variable_assignment.push_back(fin.squared());

			/* split variable assignment */
			mr1cs_primary_input<FieldT> primary_input(full_variable_assignment.begin(), full_variable_assignment.begin() + num_inputs);
			mr1cs_primary_input<FieldT> auxiliary_input(full_variable_assignment.begin() + num_inputs, full_variable_assignment.end());

			/* sanity checks */
			assert(cs.num_variables() == full_variable_assignment.size());
			assert(cs.num_variables() >= num_inputs);
			assert(cs.num_inputs() == num_inputs);
			assert(cs.num_constraints() == num_constraints);
			assert(cs.is_satisfied(primary_input, auxiliary_input));

			libff::leave_block("Call to generate_mr1cs_example_with_field_input");

			return mr1cs_example<FieldT>(std::move(cs), std::move(primary_input), std::move(auxiliary_input));
		}

	template<typename FieldT>
		mr1cs_example<FieldT> generate_mr1cs_example_with_binary_input(const size_t num_constraints,
				const size_t num_inputs)
		{
			libff::enter_block("Call to generate_mr1cs_example_with_binary_input");

			assert(num_inputs >= 1);

			mr1cs_constraint_system<FieldT> cs;
			cs.primary_input_size = num_inputs;
			cs.auxiliary_input_size = num_constraints; /* we will add one auxiliary variable per constraint */
			
			// add mr1cs value to zero
			cs.convol_outputs_size = 0;
			cs.convol_size = 0;

			mr1cs_variable_assignment<FieldT> full_variable_assignment;
			for (size_t i = 0; i < num_inputs; ++i)
			{
				full_variable_assignment.push_back(FieldT(std::rand() % 2));
			}

			size_t lastvar = num_inputs-1;
			for (size_t i = 0; i < num_constraints; ++i)
			{
				++lastvar;
				const size_t u = (i == 0 ? std::rand() % num_inputs : std::rand() % i);
				const size_t v = (i == 0 ? std::rand() % num_inputs : std::rand() % i);

				/* chose two random bits and XOR them together:
				   res = u + v - 2 * u * v
				   2 * u * v = u + v - res
				 */
				linear_combination<FieldT> A, B, C;
				linear_combination<FieldT> A2, B2, C2;
				A.add_term(u+1, 2);
				B.add_term(v+1, 1);
				if (u == v)
				{
					C.add_term(u+1, 2);
				}
				else
				{
					C.add_term(u+1, 1);
					C.add_term(v+1, 1);
				}
				C.add_term(lastvar+1, -FieldT::one());

				cs.add_constraint(mr1cs_constraint<FieldT>(A, B, C, A2, B2, C2));
				full_variable_assignment.push_back(full_variable_assignment[u] + full_variable_assignment[v] - full_variable_assignment[u] * full_variable_assignment[v] - full_variable_assignment[u] * full_variable_assignment[v]);
			}

			/* split variable assignment */
			mr1cs_primary_input<FieldT> primary_input(full_variable_assignment.begin(), full_variable_assignment.begin() + num_inputs);
			mr1cs_primary_input<FieldT> auxiliary_input(full_variable_assignment.begin() + num_inputs, full_variable_assignment.end());

			/* sanity checks */
			assert(cs.num_variables() == full_variable_assignment.size());
			assert(cs.num_variables() >= num_inputs);
			assert(cs.num_inputs() == num_inputs);
			assert(cs.num_constraints() == num_constraints);
			assert(cs.is_satisfied(primary_input, auxiliary_input));

			libff::leave_block("Call to generate_mr1cs_example_with_binary_input");

			return mr1cs_example<FieldT>(std::move(cs), std::move(primary_input), std::move(auxiliary_input));
		}

} // libsnark

#endif // R1CS_EXAMPLES_TCC
