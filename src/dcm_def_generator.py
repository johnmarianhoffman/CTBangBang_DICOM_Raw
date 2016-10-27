import jinja2

########### SPECIFY METADATA ###############
# You should only have to change info here #
# to update to a new dictionary            #
############################################
# Raw data compatible dicom dictionary
dicom_dictionary_path="/home/john/Code/CTBangBang_DICOM_Raw/resources/DICOM-CT-PD-dict_v8.txt"

# List of tags/groups we want to compile into a DCMTK-style header file (similar to dcdeftag.h)
tag_file="/home/john/Code/CTBangBang_DICOM_Raw/resources/compile_tags.txt"

########### END OF METADATA ###########

def main():
    # Load the dictionary and parse into output strings
    data={'strings':compile()}
    
    # Call jinja to actually run the templating
    loader = jinja2.FileSystemLoader(searchpath="/home/john/Code/CTBangBang_DICOM_Raw/src")
    env = jinja2.Environment(loader=loader)
    vars = {
        "data":data,
    }
    
    # Render our source
    template = env.get_template("dcdeftag_open_raw.tpl")
    with open("dcdeftag_open_raw.h","w") as f:
        f.write(template.render(vars))

def split_tag(tag):
    return tag.rstrip().strip("(").strip(")").split(",")
        
def compile():
    # Get the list of tags/group we need to add
    # Want a dictionary where each key is a group id and entries are lists of tags for that group
    requested_tag_dict={}
    with open(tag_file,'r') as f:
        tags=f.readlines()
        sanitized_tags=[]
        for t in tags:
            t=split_tag(t)
            if t[0] not in requested_tag_dict.keys():
                requested_tag_dict[t[0]]=[]
                requested_tag_dict[t[0]].append(t[1])
            else:
                requested_tag_dict[t[0]].append(t[1])

    # Load the DICOM dictionary into a searchable list
    with open(dicom_dictionary_path,'r') as f:
        dict_raw=f.readlines()
        sanitized_dict=[]
        for l in dict_raw:
            if l.startswith("#"):
                continue
            else:
                elem=l.split("#")
                if elem=="#":
                    continue
                else:
                    elem=l.rstrip().split()
                    elem[0]=split_tag(elem[0])
                    sanitized_dict.append(elem)

    # Parse the dictionary into only the tags that we want
    final_elements=[]
    for l in sanitized_dict:
        l_group=l[0][0]
        l_tag=l[0][1]
        if l_group in requested_tag_dict.keys():
            if ((l_tag in requested_tag_dict[l_group]) or (requested_tag_dict[l_group]==['*'])):
                final_elements.append(l)

    # Parse our final elements into #define strings

    def render_string(elem):
        return ("#define DCM_%s DcmTagKey(0x%s, 0x%s)\n" % (elem[2],elem[0][0],elem[0][1]))

    strings=[]
    for e in final_elements:
        strings.append(render_string(e))

    return "".join(strings)

if __name__=="__main__":
    main()
        
