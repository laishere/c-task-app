import os

def write_to_txt(folder_path, output_file):
    with open(output_file, 'w', encoding='utf-8') as f_out:
        for root, dirs, files in os.walk(folder_path):
            for file_name in files:
                file_path = os.path.join(root, file_name)
                relative_path = os.path.relpath(file_path, folder_path)
                f_out.write(f'【{relative_path}】\n')
                with open(file_path, 'r', encoding='utf-8') as f_in:
                    content = f_in.read()
                    f_out.write(content + '\n\n')

if __name__ == "__main__":
    folder_path = 'src'
    output_file = 'codes.txt'
    write_to_txt(folder_path, output_file)
