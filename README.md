# Portable, Transferable, Standardized Codeplug Description

PTSCD is an open-source codeplug standard designed for low resource radio devices.

## Current status

PTSCD is currently at the status of proposal only.

The overall architecture is not expected to evolve much in the futur. I.e. it is expected to stay a set of files used via littlefs.

What is expected to evolve over time however is the layout of data in files, as well as the content of channels, contacts, ... .

## Components

This repository contains (or will contain):
 * Documentation of the standard as markdown files
 * A simulator 
 * Examples

### Documentation

The documentation contains the specifications as detailed as possible, as of currently. This is not stable and is expected to evolve heavily in the future as discussions are held and decisions are taken.

### Simulator

The simulator is used to evaluate the impact of implementation and decisions on the underlying flash memory. Things that are expected to be evaluated are: wear, quantity of data that can be stored, ...

 
