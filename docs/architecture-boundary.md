# DiofanCAD — Architecture Boundary

## Status

- Document class: Architecture governance
- Pass: 1
- Evidence status: INITIAL / TO VALIDATE

## Objective

Prevent semantic, orchestration, or product layers from silently becoming duplicate CAD kernels or duplicate sources of truth.

## Boundary model

```text
FreeCAD / existing kernel capabilities
        │
        ├── authoritative geometry / topology where applicable
        ├── existing persistence mechanisms where reusable
        └── existing domain capabilities where reusable

DiofanCAD semantic layer
        │
        ├── engineering meaning
        ├── persistent semantic identity
        ├── cross-domain relationships
        └── diagnostics / confidence / repair metadata

DiofanCAD orchestration
        │
        ├── context
        ├── workflow composition
        ├── validation
        └── transactions

External/open subsystems
        │
        └── accessed through explicit integration boundaries
```

This is a design boundary to validate, not a claim about current code ownership.

## Ownership rules

### Geometry

DiofanCAD must not duplicate authoritative geometry solely to make semantics easier.

### Parameters

A semantic object may expose parameter meaning, but parameter ownership must remain explicit and non-duplicated.

### Identity

Persistent semantic identity may be new functionality. It must have deterministic serialization and migration semantics.

### Relationships

Cross-domain relationships may be represented as semantic edges, provided they can be reconstructed, validated and versioned.

### Diagnostics

Model Doctor may cache diagnostic results, but diagnostic cache data must never become the source of truth for model state.

### Context Engine

The Context Engine recommends relevant actions and state; it must not silently mutate the engineering model.

### Agents

Agents operate through explicit proposal → preview → permission → transaction → validation → commit/rollback flow.

## Prohibited architecture patterns

- Parallel authoritative geometry model without an approved proof.
- Silent duplication of parameters across semantic and CAD layers.
- Hidden external command execution.
- Semantic IDs that cannot survive save/reload deterministically.
- Architecture that requires permanent divergence merely because an equivalent upstream-compatible design was not investigated.

## Required architecture record for new subsystem

```text
Problem
Current owner
Proposed owner
Data owned
Data referenced
Persistence boundary
API boundary
Compatibility impact
Performance impact
Upstream strategy
Rollback strategy
Evidence
```

## Decision rule

The simpler architecture wins when it achieves equivalent measured behavior.
