#!/bin/sh
# SPDX-FileCopyrightText: 2026 Alexandr Savca
# SPDX-License-Identifier: GPL-3.0-or-later
set -eu
root=$1
fail(){ echo "test-layout-contract: $*" >&2; exit 1; }

for dir in contracts fixtures header integration support unit; do
  [ -d "$root/tests/$dir" ] || fail "missing tests/$dir"
done

if find "$root/tests" -maxdepth 1 -type f \( -name '*.cpp' -o -name '*.h' -o -name 'check_*.sh' \) | grep . >/dev/null; then
  fail 'uncategorized test source remains in tests root'
fi

for file in \
  tests/fixtures/README.md \
  tests/fixtures/authority.h \
  tests/fixtures/application.h \
  tests/fixtures/state.h \
  tests/support/test.h \
  tests/unit/error_model_test.cpp \
  tests/unit/schema_test.cpp \
  tests/header/public_header_test.cpp \
  tests/integration/installation_publication_test.cpp \
  tests/integration/upgrade_publication_test.cpp \
  tests/integration/removal_publication_test.cpp \
  tests/integration/object_translation_test.cpp \
  tests/integration/completed_evidence_test.cpp \
  tests/integration/binding_refusal_test.cpp \
  tests/integration/package_state_refusal_test.cpp \
  tests/integration/policy_outcome_test.cpp \
  tests/integration/state_projection_test.cpp \
  tests/integration/state_projection_refusal_test.cpp \
  tests/integration/state_publication_projection_test.cpp; do
  [ -s "$root/$file" ] || fail "missing $file"
done

for suite in unit integration header contract; do
  grep -F "suite: '$suite'" "$root/tests/meson.build" >/dev/null ||
    fail "$suite suite is not registered"
done
