#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <ratio>

/// @brief Measures wall-clock time from construction
/// @note Uses a steady clock, so adjustments to the system time cannot
///       skew a measurement mid-render
class Timer final
{
  public:
	/// @brief Restarts the timer from now
	void reset() { beg = Clock::now(); }

	/// @brief Returns the seconds elapsed since construction or the last
	///        reset()
	[[nodiscard]] double elapsed() const
	{
		return std::chrono::duration_cast<Second>(Clock::now() - beg).count();
	}

  private:
	using Clock	 = std::chrono::steady_clock;
	using Second = std::chrono::duration<double, std::ratio<1>>;

	std::chrono::time_point<Clock> beg{Clock::now()};
};

#endif // TIMER_H
