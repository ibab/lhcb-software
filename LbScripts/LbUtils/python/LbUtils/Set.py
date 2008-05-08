""" pickable set. Doesn't require a __hash__ function for its elements """


class Set(list):
    def __init__(self):
        super(Set, self).__init__()
    def add(self,element):
        if element not in self:
            self.append(element)
    def __contains__(self, element):
        for e in self:
            if e == element :
                return 1
        else:
            return 0
    def __or__(self,other):
        union = Set()
        for s in self:
            union.add(s)
        for o in other:
            union.add(o)
        return union
    def __ior__(self, other):
        for o in other:
            self.add(o)
        return self
