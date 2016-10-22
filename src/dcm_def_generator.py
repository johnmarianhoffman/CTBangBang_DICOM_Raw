import jinja2

########### SPECIFY METADATA ###############
# You should only have to change info here #
# to update to a new dictionary            #
############################################
# Raw data compatible dicom dictionary
dicom_dictionary_path="/home/john/Code/open_dicom_reader/resources/DICOM-CT-PD-dict_v8.txt"

# List of tags/groups we want to compile into a DCMTK-style header file (similar to dcdeftag.h)
tag_file="/home/john/Code/open_dicom_reader/resources/compile_tags.txt"

########### END OF METADATA ###########

def main():
    # Load the dictionary and parse into output strings
    s=compile()
    
    # Call jinja to actually run the templating
    loader = jinja2.FileSystemLoader(searchpath=template_path)
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
    with open(tag_file,'r') as f:
        tags=f.readlines()
        sanitized_tags=[]
        for t in tags:            
            sanitized_tags.append(split_tag(t))
    print(sanitized_tags)


    

    # Load the dictionary into a searchable list
    with open(dicom_dictionary_path,'r') as f:
        dict_raw=f.readlines()
        sanitized_dict=[]
        for l in dict_raw:
            elem=l.split("#")
            if elem=="#":
                continue
            else:
                elem=l.rstrip().split()
                elem[0]=split_tag(elem[0])
                sanitized_dict.append(elem)

    
                
    # Extract lines from dict matching
    requested_groups=[]
    requested_tags=[]
    for t in sanitized_tags:
        requested_groups.append(t[0])
        requested_tags.append(t[1])

    # Filter all lines that match the group
    matched_lines=[]
    for l in sanitized_dict:
        if (l[0] in requested_groups) && :
            matched_lines.append[l]

    
                
    print('butt')

    return define_strings

if __name__=="__main__":
    main()
        
