local TreeItem = FLua.Class("TreeItem")

local NodeData = require "frames.treeview.NodeData"

function TreeItem.new(data, parent)
    local obj = TreeItem()
    obj.data_ = data or NodeData()
    obj.parent_ = parent
	return obj
end
function TreeItem:__constructor()
    self.children_ = {}
    self.data_ = NodeData()
    self.parent_ = nil
end
function TreeItem:__destructor() 
end
function TreeItem:toString()
    return string.format("[%s] num_children:%d, folder:%s", self:GetPointer(), self:num_children(), tostring(self:folder()))
end
function TreeItem:data()
    return self.data_
end
function TreeItem:num_children()
    return #(self.children_)
end
function TreeItem:child(index)
    return self.children_[index]
end
function TreeItem:has_children()
    return self:num_children() > 0
end
function TreeItem:add_child(child)
    child:set_parent(self); 
	table.insert(self.children_, child) 
end
function TreeItem:remove_child(child)    
    for i=1, self:num_children() do
        if child == self:child(i) then
            table.remove(self.children_, i)
            break
        end
    end
end
function TreeItem:get_last_child()
    if self:has_children() then
		return self:child(self:num_children()):get_last_child()
	end
	return self
end
function TreeItem:parent()
    return self.parent_
end
function TreeItem:folder()
    return self.data_.folder_
end
function TreeItem:set_parent(parent)
    self.parent_ = parent
end

return TreeItem