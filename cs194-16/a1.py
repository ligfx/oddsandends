#!/usr/bin/python
"""
CS194-16 Data Science Assignment 1
UC Berkeley
"""

from __future__ import division
import math
import os
import sys

import numpy as np
from pandas import *

class CFData:
    def __init__(self, df):
        self.value = df
    def where_eq(self, k, v):
        return CFData(self.value[self.value[k] == v])
    
    def all_states(self):
        return self.value.groupby("contbr_st").groups.keys()
    def state(self, st):
        return self.where_eq('contbr_st', st)
    def donations(self):
        return self.where_eq('form_tp', "SA17A")
    def refunds(self):
        return self.where_eq('form_tp', "SB28A")
    def num_records(self):
        return self.value.shape[0]
    def num_contributors(self):
        return len(self.value["contbr_nm"].unique())
    def num_bipartisan_donors(self):
        people_parties = self.value.filter(("party", "contbr_nm")).drop_duplicates()
        grouped = people_parties.groupby("contbr_nm")
        return len(filter(lambda x: len(x) > 1, grouped.groups.values()))
    def total_amount(self):
        return self.value['contb_receipt_amt'].sum()
    def parties(self):
        return self.value['party'].unique()
    def group_by_candidate(self):
        return CFGroups(self.value.groupby('cand_nm'))
    def group_by_party(self):
        return CFGroups(self.value.groupby('party'))
    def group_by_contributor(self):
        return CFGroups(self.value.groupby('contbr_nm'))
    def top_by(self, groupby, propname):
        groups = getattr(self, "group_by_" + groupby)()
        total = float(getattr(self, propname)())
        return getattr(groups, propname)().fractionalize(total).max()

class CFGroups:
    def __init__(self, groups):
        self.groups = groups
    def reduce(self, fun):
        return CFDict((name, fun(CFData(group))) for name, group in self.groups)
    
    def num_records(self):
        return self.reduce(lambda group: group.num_records())
    def num_contributors(self):
        return self.reduce(lambda group: group.num_contributors())
    def total_amount(self):
        return self.reduce(lambda group: group.total_amount())

class CFDict:
    def __init__(self, tuples):
        self.value = tuples
    def map(self, fun):
        return CFDict(fun(name, value) for name, value in self.value)
    def filter(self, fun):
        return CFDict((name, value) for name, value in self.value if fun(value))
    def fractionalize(self, total):
        return self.map(lambda name, value: (name, value/total))
    def max(self):
        try:
            return max(self.value, key = lambda x: x[1])
        except ValueError:
            return (None, 0.0)
    def __len__(self):
        return len(list(self.value))

def add_party_column(df):
    """Add a column representing the candidate's party to the data frame.

    Args:
        df: A DataFrame generated from the campaign finance data csv file.

    Returns:
        A DataFrame based on df with an additional column keyed "party" whose
        values represent the party of the candidates.
        For Democratic candidates, use value "Democrat".
        For Republicans, use "Republican".
        For Libertarian candidates, use value "Libertarian".
    """
    r = "Republican"
    d = "Democrat"
    l = "Libertarian"
    # print "\n".join(data['cand_nm'].unique())
    candidates_parties = {
        'Bachmann, Michelle': r,
        'Romney, Mitt': r,
        'Obama, Barack': d,
        "Roemer, Charles E. 'Buddy' III": r,
        'Pawlenty, Timothy': r,
        'Johnson, Gary Earl': l,
        'Paul, Ron': r,
        'Santorum, Rick': r,
        'Cain, Herman': r,
        'Gingrich, Newt': r,
        'McCotter, Thaddeus G': r,
        'Huntsman, Jon': r,
        'Perry, Rick': r
    }
    df['party'] = df['cand_nm'].map(lambda name: candidates_parties[name])
    return df

def bucketize_donation_amounts(df):
    """Generate a histogram for the donation amount.

    Put donation amount into buckets and generate a histogram for these buckets.
    The buckets are: (0, 1], (1, 10], (10, 100], (100, 1000], (1000, 10000],
    (10000, 100000], (100000, 1000000], (1000000, 10000000].

    Args:
        df: A DataFrame generated from the campaign finance data csv file.

    Returns:
        A list containing 8 integers. The Nth integer is the count of donation
        amounts that fall into the Nth bucket. E.g. [2, 3, 4, 5, 4, 3, 1, 1].
    """
    table = CFData(df).donations().value
    buckets = [0, 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000]
    table['bucket'] = np.digitize(table['contb_receipt_amt'], buckets)
    return list(table.pivot_table('contb_receipt_amt', rows='bucket', aggfunc=len))[1:]

def main(*args):
    if os.path.exists("data/P00000001-ALL.pandas"):
        print "Reading data/P00000001-ALL.pandas..."
        df = DataFrame().load("data/P00000001-ALL.pandas")
    else:
        print "Reading data/P00000001-ALL.csv..."
        df = read_csv('data/P00000001-ALL.csv')
        add_party_column(df)
        print "Saving data/P00000001-ALL.pandas..."
        df.save("data/P00000001-ALL.pandas")
    
    scoped = CFData(df).donations()
    for state in scoped.all_states():
        print state
        data = scoped.state(state)
        print "Donors: " + str(data.num_contributors())
        # print "Bipartisan donors: " + str(data.num_bipartisan_donors())
        print "Donations: " + str(data.num_records())
        print "Top candidate by amount: " + str(data.top_by("candidate", "total_amount"))
        print "Top candidate by donors: " + str(data.top_by("candidate", "num_contributors"))
        print "Top party by amount: " + str(data.top_by("party", "total_amount"))
        print "Top party by donors: " + str(data.top_by("party", "num_contributors"))

    # TODO: Put the code you used to explore the data set here.


if __name__ =='__main__':
    sys.exit(main(*sys.argv[1:]))

