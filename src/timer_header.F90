module timer_header

  use constants, only: ZERO

  implicit none

!===============================================================================
! TIMER represents a timer that can be started and stopped to measure how long
! different routines run. The intrinsic routine system_clock is used to measure
! time rather than cpu_time.
!===============================================================================

  type Timer
    private
    logical :: running      = .false. ! is timer running?
    integer(8) :: start_counts = 0       ! counts when started
    real(8), public :: elapsed = ZERO ! total time elapsed in seconds
  contains
    procedure :: start     => timer_start
    procedure :: get_value => timer_get_value
    procedure :: stop      => timer_stop
    procedure :: reset     => timer_reset
  end type Timer

contains

!===============================================================================
! TIMER_START starts running a timer and measures the current time
!===============================================================================

  subroutine timer_start(self)
    class(Timer), intent(inout) :: self

    ! Turn timer on and measure starting time
    self % running = .true.
    call system_clock(self % start_counts)
  end subroutine timer_start

!===============================================================================
! TIMER_GET_VALUE returns the current value of the timer
!===============================================================================

  function timer_get_value(self) result(elapsed)
    class(Timer), intent(in) :: self   ! the timer
    real(8)                 :: elapsed ! total elapsed time

    integer(8) :: end_counts   ! current number of counts
    integer(8) :: count_rate   ! system-dependent counting rate
    real    :: elapsed_time ! elapsed time since last start

    if (self % running) then
      call system_clock(end_counts, count_rate)
      elapsed_time = real(end_counts - self % start_counts, 8) / &
           real(count_rate, 8)
      elapsed = self % elapsed + elapsed_time
    else
      elapsed = self % elapsed
    end if
  end function timer_get_value

!===============================================================================
! TIMER_STOP stops the timer and sets the elapsed time
!===============================================================================

  subroutine timer_stop(self)
    class(Timer), intent(inout) :: self

    ! Check to make sure timer was running
    if (.not. self % running) return

    ! Stop timer and add time
    self % elapsed = self % get_value()
    self % running = .false.
  end subroutine timer_stop

!===============================================================================
! TIMER_RESET resets a timer to have a zero value
!===============================================================================

  pure subroutine timer_reset(self)
    class(Timer), intent(inout) :: self

    self % running      = .false.
    self % start_counts = 0
    self % elapsed      = ZERO
  end subroutine timer_reset

end module timer_header
