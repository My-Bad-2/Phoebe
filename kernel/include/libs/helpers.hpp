#ifndef LIBS_HELPERS_HPP
#define LIBS_HELPERS_HPP

#include <algorithm>
#include <ranges>

/**
 * @brief Erases elements equal to a specified value from a container.
 *
 * This function removes all occurrences of the specified value from the 
 * container by using `std::remove` and then erasing the elements from the 
 * container. The container must support the `erase` member function.
 *
 * @param container The container from which to erase elements.
 * @param val The value to be removed from the container.
 * @return `true` if any elements were removed; otherwise, `false`.
 */
inline constexpr bool erase_from(auto&& container, auto&& val)
{
    return container.erase(std::remove(std::ranges::begin(container), std::ranges::end(container),
                                       std::move(val)),
                           std::ranges::end(container)) != std::ranges::end(container);
}

/**
 * @brief Erases elements from a container based on a predicate.
 *
 * This function removes elements from the container for which the predicate 
 * returns `true` by using `std::remove_if` and then erasing the elements from 
 * the container. The container must support the `erase` member function.
 *
 * @param container The container from which to erase elements.
 * @param pred The predicate used to determine which elements to remove.
 * @return `true` if any elements were removed; otherwise, `false`.
 */
inline constexpr bool erase_from_if(auto&& container, auto pred)
{
    return container.erase(
               std::remove_if(std::ranges::begin(container), std::ranges::end(container), pred),
               std::ranges::end(container)) != std::ranges::end(container);
}

/**
 * @brief Checks if a container contains a specified value.
 *
 * This function uses `std::find` to check if the container contains at least 
 * one element equal to the specified value.
 *
 * @param container The container to search.
 * @param val The value to find in the container.
 * @return `true` if the value is found; otherwise, `false`.
 */
inline constexpr bool contains(auto&& container, auto&& val)
{
    return std::find(std::ranges::begin(container), std::ranges::end(container), std::move(val)) !=
           std::ranges::end(container);
}

/**
 * @brief Checks if a container contains an element that satisfies a predicate.
 *
 * This function uses `std::find_if` to check if there exists at least one 
 * element in the container for which the predicate returns `true`.
 *
 * @param container The container to search.
 * @param pred The predicate used to evaluate elements in the container.
 * @return `true` if an element satisfying the predicate is found; otherwise, `false`.
 */
inline constexpr bool contains_if(auto&& container, auto pred)
{
    return std::find_if(std::ranges::begin(container), std::ranges::end(container), pred) !=
           std::ranges::end(container);
}

#endif // LIBS_HELPERS_HPP