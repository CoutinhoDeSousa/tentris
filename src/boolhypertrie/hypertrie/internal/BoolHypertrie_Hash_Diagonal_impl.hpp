#ifndef HYPERTRIE_BOOLHYPERTRIE_HASH_DIAGONAL_HPP
#define HYPERTRIE_BOOLHYPERTRIE_HASH_DIAGONAL_HPP

#include <functional>
#include <exception>

#include "hypertrie/internal/util/CONSTANTS.hpp"

#include "hypertrie/internal/container/TslMap.hpp"
#include "hypertrie/internal/container/BoostFlatSet.hpp"
#include "hypertrie/internal/BoolHypertrie_impl.hpp"

namespace hypertrie::internal {


	template<typename key_part_type, template<typename, typename> class map_type,
			template<typename> class set_type>
	class Diagonal {

		using const_BoolHypertrie_t = const_BoolHypertrie<key_part_type, map_type, set_type>;
		template<pos_type depth>
		using RawBoolHypertrie = typename const_BoolHypertrie_t::template RawBoolHypertrie<depth>;
		using Key = typename const_BoolHypertrie_t::Key;
		using SliceKey = typename const_BoolHypertrie_t::SliceKey;
		template<pos_type depth, pos_type diag_depth>
		using RawDiagonal =  typename hypertrie::internal::interface::rawboolhypertrie<key_part_type, container::tsl_sparse_map, container::boost_flat_set>::template RawDiagonal<diag_depth, depth>;

		struct RawDiagFunctions {
			void (*init)(void *);

			key_part_type (*currentKeyPart)(void const *);

			const_BoolHypertrie_t (*currentValue)(void const *);

			const_BoolHypertrie_t (*getValueByKeyPart)(void const *, key_part_type);

			bool (*contains)(void const *, key_part_type);

			void (*inc)(void *);

			bool (*empty)(void const *);

			size_t (*size)(void const *);
		};


		template<pos_type depth, pos_type diag_depth_>
		inline static RawDiagFunctions getRawDiagFunctions() {
			return RawDiagFunctions{
					&RawDiagonal<diag_depth_, depth>::init,
					&RawDiagonal<diag_depth_, depth>::currentKeyPart,
					[]([[maybe_unused]]void const *diag_ptr) -> const_BoolHypertrie_t {
						if constexpr (depth > diag_depth_) {
							return const_BoolHypertrie_t(RawDiagonal<diag_depth_, depth>::currentValue(diag_ptr));
						} else {
							throw std::invalid_argument{"currentValue is only implemented for depth > diag_depth"};
						}
					},
					[]([[maybe_unused]]void const *diag_ptr,
					   [[maybe_unused]]key_part_type key_part) -> const_BoolHypertrie_t {
						if constexpr (depth > diag_depth_) {
							auto raw_result = RawDiagonal<diag_depth_, depth>::getValueByKeyPart(diag_ptr, key_part);
							if (raw_result)

								return const_BoolHypertrie_t(raw_result);
							else
								return const_BoolHypertrie_t(depth - diag_depth_);
						} else {
							throw std::invalid_argument{"currentValue is only implemented for depth > diag_depth"};
						}
					},
					[]([[maybe_unused]]void const *diag_ptr, [[maybe_unused]]key_part_type key_part) -> bool {
						if constexpr (depth > diag_depth_) {
							throw std::invalid_argument{"currentValue is only implemented for depth > diag_depth"};
						} else {
							return bool(RawDiagonal<diag_depth_, depth>::getValueByKeyPart(diag_ptr, key_part));
						}
					},
					&RawDiagonal<diag_depth_, depth>::inc,
					&RawDiagonal<diag_depth_, depth>::empty,
					&RawDiagonal<diag_depth_, depth>::size
			};
		}

		inline static std::vector<std::vector<RawDiagFunctions>> functions{
				{
						getRawDiagFunctions<1, 1>()
				},
				{
						getRawDiagFunctions<2, 1>(),
						getRawDiagFunctions<2, 2>()
				},
				{
						getRawDiagFunctions<3, 1>(),
						getRawDiagFunctions<3, 2>(),
						getRawDiagFunctions<3, 3>()
				},
				{
						getRawDiagFunctions<4, 1>(),
						getRawDiagFunctions<4, 2>(),
						getRawDiagFunctions<4, 3>(),
						getRawDiagFunctions<4, 4>(),
				},
				{
						getRawDiagFunctions<5, 1>(),
						getRawDiagFunctions<5, 2>(),
						getRawDiagFunctions<5, 3>(),
						getRawDiagFunctions<5, 4>(),
						getRawDiagFunctions<5, 5>(),
				}
		};

	public:
		using poss_type = std::vector<pos_type>;
	private:

		std::shared_ptr<void> raw_diag;
		RawDiagFunctions *raw_diag_funcs;

		template<pos_type diag_depth_, pos_type depth>
		static inline std::shared_ptr<void>
		getRawDiagonal(const const_BoolHypertrie_t &boolhypertrie, [[maybe_unused]]const poss_type &positions) {
			if constexpr (depth == diag_depth_) {
				const auto &raw_boolhypertrie = *(static_cast<RawBoolHypertrie<depth> const *>(boolhypertrie.hypertrie.get()));
				return std::make_shared<RawDiagonal<diag_depth_, depth>>(raw_boolhypertrie);
			} else {
				const auto &raw_boolhypertrie = *(static_cast<RawBoolHypertrie<depth> const *>(boolhypertrie.hypertrie.get()));
				return std::make_shared<RawDiagonal<diag_depth_, depth>>(raw_boolhypertrie, positions);
			}
		}

		static inline std::shared_ptr<void>
		getRawDiagonal(const const_BoolHypertrie_t &boolhypertrie, const poss_type &positions) {
			switch (boolhypertrie.depth()) {
				case 1: {
					return getRawDiagonal<1, 1>(boolhypertrie, positions);
				}
				case 2: {
					switch (positions.size()) {
						case 1: {
							return getRawDiagonal<1, 2>(boolhypertrie, positions);
						}
						case 2: {
							return getRawDiagonal<2, 2>(boolhypertrie, positions);
						}
						default:
							break;
					}
					break;
				}
				case 3: {
					switch (positions.size()) {
						case 1: {
							return getRawDiagonal<1, 3>(boolhypertrie, positions);
						}
						case 2: {
							return getRawDiagonal<2, 3>(boolhypertrie, positions);
						}
						case 3: {
							return getRawDiagonal<3, 3>(boolhypertrie, positions);
						}
						default:
							break;
					}
					break;
				}
				case 4: {
					switch (positions.size()) {
						case 1: {
							return getRawDiagonal<1, 4>(boolhypertrie, positions);
						}
						case 2: {
							return getRawDiagonal<2, 4>(boolhypertrie, positions);
						}
						case 3: {
							return getRawDiagonal<3, 4>(boolhypertrie, positions);
						}
						case 4: {
							return getRawDiagonal<4, 4>(boolhypertrie, positions);
						}
						default:
							break;
					}
					break;
				}
				case 5: {
					switch (positions.size()) {
						case 1: {
							return getRawDiagonal<1, 5>(boolhypertrie, positions);
						}
						case 2: {
							return getRawDiagonal<2, 5>(boolhypertrie, positions);
						}
						case 3: {
							return getRawDiagonal<3, 5>(boolhypertrie, positions);
						}
						case 4: {
							return getRawDiagonal<4, 5>(boolhypertrie, positions);
						}
						case 5: {
							return getRawDiagonal<5, 5>(boolhypertrie, positions);
						}
						default:
							break;
					}
					break;
				}
				default:
					break;
			}
			throw std::logic_error{"not implemented."};
		}

	public:

		Diagonal() = default;

		Diagonal(Diagonal &) = default;

		Diagonal(const Diagonal &) = default;

		Diagonal(Diagonal &&) noexcept = default;

		Diagonal &operator=(Diagonal &&) = default;

		Diagonal &operator=(const Diagonal &) = default;


		Diagonal(const_BoolHypertrie_t const *const boolhypertrie, const poss_type &positions) :
				raw_diag(getRawDiagonal(*boolhypertrie, positions)),
				raw_diag_funcs(&functions[boolhypertrie->depth() - 1][positions.size() - 1]) {}

		Diagonal(const const_BoolHypertrie_t &boolhypertrie, const poss_type &positions) :
				Diagonal(&boolhypertrie, positions) {}


		/*
		* Potentially forwards the Diagonal and leafs it in a safe state. <br/>
		* It checks if the current key_part is valid and increments it until it is valid.
		*/
		void init() const { // #
			std::invoke(raw_diag_funcs->init, raw_diag.get());
		}

		/*
		* Must only be called in a safe state. <br/>
		* Returns the current value.
		*/
		[[nodiscard]]
		key_part_type currentKeyPart() const { // #
			return std::invoke(raw_diag_funcs->currentKeyPart, raw_diag.get());
		}

		[[nodiscard]]
		const_BoolHypertrie_t currentValue() const {
			return std::invoke(raw_diag_funcs->currentValue, raw_diag.get());
		}

		/**
		 * use only if the diagonal is not calculated over all dimensions.
		 * @param key_part
		 * @return
		 */
		[[nodiscard]]
		const_BoolHypertrie_t operator[](key_part_type key_part) {
			return std::invoke(raw_diag_funcs->getValueByKeyPart, raw_diag.get(), key_part);
		}

		/**
		 * use only if the diagonal is calculated over all dimensions.
		 * @param key_part
		 * @return
		 */
		[[nodiscard]]
		bool contains(key_part_type key_part) {
			return std::invoke(raw_diag_funcs->contains, raw_diag.get(), key_part);
		}

		/*
		* Forwards the Diagonal and leafs it in a safe state. <br/>
		* Increments the diagonal to the next valid key_part.
		*/
		void operator++() { // #
			return std::invoke(raw_diag_funcs->inc, raw_diag.get());
		}

		/*
		* If it returns true there are no key_parts left for sure.
		* Otherwise there are potential key_parts left and therefore, there may also be valid key_parts left.
		* @return
		*/
		[[nodiscard]]
		bool empty() const { // #
			return std::invoke(raw_diag_funcs->empty, raw_diag.get());
		}

		/*
		* Always safe. <br/>
		* Get the number of potential key_parts. This is a upper bound to the valid key_parts.
		* @return number of potential key_parts
		*/
		[[nodiscard]]
		size_t size() const {
			return std::invoke(raw_diag_funcs->size, raw_diag.get());
		}

		bool operator<(const Diagonal &other) const {
			return this->size() < other.size();
		}
	};
}

#endif //HYPERTRIE_BOOLHYPERTRIE_HASH_DIAGONAL_HPP
