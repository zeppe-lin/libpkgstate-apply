// SPDX-FileCopyrightText: 2026 Alexandr Savca
// SPDX-License-Identifier: GPL-3.0-or-later
#include <libpkgstate-apply/adapter.h>
#include <libpkgstate-apply/state_projection.h>

#include <cstddef>

namespace {
template<class T, std::size_t Size, std::size_t Align>
constexpr void require_layout()
{
  static_assert(sizeof(T) == Size, "ABI size changed");
  static_assert(alignof(T) == Align, "ABI alignment changed");
}
}

int main()
{
#if defined(__x86_64__) || defined(_M_X64)
  require_layout<pkgapply::lease_bound_state_projection, 296, 8>();
  require_layout<pkgstate::apply_adapter::projection_error, 24, 8>();
  require_layout<
      pkgstate::apply_adapter::application_state_projection_error, 24, 8>();
  require_layout<pkgstate::apply_adapter::lease_bound_application_state, 632, 8>();
#endif
  return 0;
}
