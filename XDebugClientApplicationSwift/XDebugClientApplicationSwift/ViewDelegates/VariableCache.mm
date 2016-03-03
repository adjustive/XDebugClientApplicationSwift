//
//  Variable.m
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/6/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "VariableCache.h"
#import "../XDebugClient/Model/Variable.hpp"
#import "../XDebugClient/XDebugSession.hpp"
#import <string>
#import <vector>
#import <iostream>

@implementation VariableCache
{
    NSString* name;
    NSString* type;
    NSString* value_display;
    int number_of_children;
    BOOL is_expanded;
    BOOL is_init;
    NSMutableArray<VariableCache*>* children;
    void* c_pointer;
}

- (id) initWithCPointer:(void*)ptr
{
    self = [super init];
    c_pointer = ptr;
    return self;
}

- (id) initRootWithVariablesPointer:(void*)ptr
{
    self = [super init];
    if (ptr)
    {
        c_pointer = ptr;
        is_expanded = true;
    }
    return self;
}

- (void) fetchData
{
    @synchronized(self) {
        [self _fetchData];
    }
}

- (void) _fetchData
{
    if (is_init || !c_pointer)
    {
        return;
    }
    // fetch data from bridge
    is_init = true;
    Variable* v = (Variable*)c_pointer;
    std::string c_name = v->getName();
    std::string c_type = v->getType();
    std::string c_value = v->getValueMaxChars(255);
    name = [[NSString alloc] initWithCString:c_name.c_str() encoding:NSUTF8StringEncoding];
    type = [[NSString alloc] initWithCString:c_type.c_str() encoding:NSUTF8StringEncoding];
    value_display = [[NSString alloc] initWithCString:c_value.c_str() encoding:NSUTF8StringEncoding];
    number_of_children = v->num_children;
    is_expanded = v->is_expanded;
}

- (void) fetchChildren
{
    [self fetchData];
    @synchronized(self) {
        [self _fetchChildren];
    }
}

- (void) _fetchChildren
{
    if (children || !c_pointer || !number_of_children)
    {
        return;
    }
    // fetch data from bridge
    children = [[NSMutableArray alloc] initWithCapacity:number_of_children];
    Variable* v = (Variable*)c_pointer;
    std::vector<Variable*>* c_children = v->getChildElementsReference();
    if (!c_children)
    {
        return;
    }
    for (int i = 0; i<c_children->size();i++)
    {
        Variable* v_ptr_i = c_children->at(i);
        VariableCache* obj_i = [[VariableCache alloc] initWithCPointer:(void*)v_ptr_i];
        [children addObject:obj_i];
    }
}

- (NSString*) getName
{
    [self fetchData];
    return name;
}

- (NSString*) getValueDisplay
{
    [self fetchData];
    return value_display;
}

- (NSString*) getType
{
    [self fetchData];
    return type;
}

- (NSString*) getFullValue
{
    [self fetchData];
    return @"_not_implemented";
}

- (int) getNumberOfChildren
{
    [self fetchData];
    return number_of_children;
}

- (VariableCache*) getChildAtIndex: (int) index
{
    [self fetchChildren];
    if (children && index < children.count)
    {
        return (VariableCache*)[children objectAtIndex:index];
    } else {
        return nil;
    }
}

- (void) addToExpansionArray: (NSMutableArray*)expansion_array
{
    [self fetchData];
    if (is_expanded)
    {
        [expansion_array addObject:self];
        [self fetchChildren];
        for (int i = 0; i<children.count;i++)
        {
            VariableCache* child_i = [children objectAtIndex:i];
            [child_i addToExpansionArray:expansion_array];
            
        }
    }
}


- (void) updateExpandedState: (BOOL)is_expanded_cpy withSharedPointer:(void*)shared_ptr
{
    if (shared_ptr && c_pointer)
    {
        std::shared_ptr<BreakpointState>* a = (std::shared_ptr<BreakpointState>*)shared_ptr;
        BreakpointState* c = a->get();
        if (c)
        {
            Variable* v = (Variable*)c_pointer;
            is_expanded = is_expanded_cpy;
            if (is_expanded)
            {
                v->updateExpansionState(c, true);
            } else {
                v->updateExpansionState(c, false);
            }
        }
    }
}

@end