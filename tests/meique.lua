
function addTestTarget(file)
    local t = Executable:new(string.sub(file, 0, -5))
    t:useQtAutomoc()
    t:use(qtTest)
    t:addCustomFlags("-fPIC")
    t:addFile(file)
    -- TODO: Use RubyScanner as a static library to avoid double compilation
    t:addFiles("../editor/RubyScanner.cpp")
    t:addIncludePath("../editor")
    addTest(t)
end

tests = [[
    ScannerTest.cpp
]]

string.gsub(tests, '([^%s]+)', addTestTarget)
