import logging
import os

def get_availalbe_tests(tests_dir: str) -> tuple[list[int], dict[int, str]]:
    """
    Searches 'tests_dir' for folders with names like 'xxxx_*' where 'xxxx' is a number.
    Returns a tuple of two lists:
        1. List of test numbers
        2. Dictionary of test numbers to test paths
    """

    tests_number_to_path_map = {}
    tests_availalbe = []
    
    logging.info('Searching for tests in: %s', tests_dir)
    for test_dir in os.listdir(tests_dir):
        if not os.path.isdir(os.path.join(tests_dir, test_dir)):
            logging.info('Skipping non-directory: %s', test_dir)
            continue
    
        if not test_dir.split('_')[0].isdigit():
            logging.info('Skipping non-test directory: %s', test_dir)
            continue
    
        logging.info('Found test directory: %s', test_dir)
        test_number = int(test_dir.split('_')[0])
        tests_number_to_path_map[test_number] = os.path.join(tests_dir, test_dir)
        tests_availalbe.append(test_number)
    
    tests_availalbe.sort()
    
    return tests_availalbe, tests_number_to_path_map


def compare_files(actual_outputs_path: str, expected_outputs_path: str) -> bool:
    """
    Compares two files line by line.
    Returns True if the files are identical, False otherwise.
    """

    to_compare = [
        (actual_outputs_path + '/stdout', expected_outputs_path + '/stdout'),
        (actual_outputs_path + '/stderr', expected_outputs_path + '/stderr'),
        (actual_outputs_path + '/code', expected_outputs_path + '/code'),
    ]

    for actual_output_path, expected_output_path in to_compare:
        with open(actual_output_path, 'r') as actual_output_file:
            actual_output = actual_output_file.read()
        
        with open(expected_output_path, 'r') as expected_output_file:
            expected_output = expected_output_file.read()
        
        if actual_output != expected_output:
            return False
    
    return True


def copy_dir_contains(src_dir: str, dst_dir: str) -> None:
    """
    Copies all files and directories from 'src_dir' to 'dst_dir'.
    """
    try:
        os.mkdir(dst_dir)
    except FileExistsError:
        pass
    for src_file in os.listdir(src_dir):
        src_file_path = os.path.join(src_dir, src_file)
        dst_file_path = os.path.join(dst_dir, src_file)
        if os.path.isdir(src_file_path):
            try:
                os.mkdir(dst_file_path)
            except FileExistsError:
                pass
            copy_dir_contains(src_file_path, dst_file_path)
        else:
            os.system(f'cp {src_file_path} {dst_file_path}')