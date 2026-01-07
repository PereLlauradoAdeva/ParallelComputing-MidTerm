import pandas as pd
import matplotlib.pyplot as plt
import os

def generate_figure_1():
    file1 = 'performance_results_1.csv'
    file2 = 'performance_results_2.csv'
    
    if not os.path.exists(file1) or not os.path.exists(file2):
        print("Error: CSV files not found.")
        return

    df1 = pd.read_csv(file1)
    df2 = pd.read_csv(file2)

    # Filter for Static schedule
    data1 = df1[df1['Schedule'] == 'Static']
    data2 = df2[df2['Schedule'] == 'Static']

    plt.figure(figsize=(10, 6))

    # Plot Dataset 1
    plt.plot(data1['Threads'], data1['Speedup'], marker='o', label='Dataset 1 (Large) - Static')

    # Plot Dataset 2
    plt.plot(data2['Threads'], data2['Speedup'], marker='s', label='Dataset 2 (Small) - Static')

    # Plot Ideal Speedup
    # Ideally speedup = threads. Using a range explicitly to match x-axis ticks roughly but keep it diagonal
    max_threads = max(data1['Threads'].max(), data2['Threads'].max())
    plt.plot([1, max_threads], [1, max_threads], 'k--', alpha=0.5, label='Ideal Speedup')

    plt.xscale('log', base=2) # Optional: Log scale can look better for 1,2,4,8,16,32, but linear is also fine. 
                               # Requests often imply linear spacing on axis but discrete ticks. 
                               # Let's stick to linear axis but with specific ticks as requested.
    plt.xscale('linear') 
    
    plt.xlabel('Number of Threads')
    plt.ylabel('Speedup (S)')
    plt.title('Speedup vs. Threads')
    plt.legend()
    plt.grid(True)
    plt.xticks([1, 2, 4, 8, 16, 32])
    
    # Add caption as text below or just save it. Usually captions are for the document, not embedded in the image.
    # I will just save the image.
    
    output_file = 'Figure_1_Speedup.png'
    plt.savefig(output_file)
    print(f"Generated {output_file}")
    plt.close()

def generate_figure_2():
    file1 = 'performance_results_1.csv'
    
    if not os.path.exists(file1):
        print(f"Error: {file1} not found.")
        return

    df = pd.read_csv(file1)

    # Filter data
    static_data = df[df['Schedule'] == 'Static']
    dynamic_data = df[df['Schedule'] == 'Dynamic']

    plt.figure(figsize=(10, 6))

    # Plot Static
    plt.plot(static_data['Threads'], static_data['Parallel_Time_ms'], marker='o', label='Static Schedule')

    # Plot Dynamic
    plt.plot(dynamic_data['Threads'], dynamic_data['Parallel_Time_ms'], marker='s', label='Dynamic Schedule')

    plt.xlabel('Number of Threads')
    plt.ylabel('Execution Time (ms)')
    plt.title('Static vs. Dynamic Comparison (Dataset 1)')
    plt.legend()
    plt.grid(True)
    plt.xticks([1, 2, 4, 8, 16, 32])
    
    # Optional: Start y-axis at 0 or let it autoscale. Autoscale usually better for time.
    
    output_file = 'Figure_2_Static_vs_Dynamic.png'
    plt.savefig(output_file)
    print(f"Generated {output_file}")
    plt.close()

def generate_figure_3():
    file1 = 'performance_results_1.csv'
    file2 = 'performance_results_2.csv'
    
    if not os.path.exists(file1) or not os.path.exists(file2):
        print("Error: CSV files not found.")
        return

    df1 = pd.read_csv(file1)
    df2 = pd.read_csv(file2)

    # Filter for Static schedule
    data1 = df1[df1['Schedule'] == 'Static']
    data2 = df2[df2['Schedule'] == 'Static']

    plt.figure(figsize=(10, 6))

    # Plot Dataset 1
    plt.plot(data1['Threads'], data1['Efficiency'], marker='o', label='Dataset 1 (Large) - Static')

    # Plot Dataset 2
    plt.plot(data2['Threads'], data2['Efficiency'], marker='s', label='Dataset 2 (Small) - Static')

    # Plot Ideal Efficiency (y=1.0)
    max_threads = max(data1['Threads'].max(), data2['Threads'].max())
    plt.axhline(y=1.0, color='k', linestyle='--', alpha=0.5, label='Ideal Efficiency')

    plt.xlabel('Number of Threads')
    plt.ylabel('Efficiency (E)')
    plt.title('Parallel Efficiency vs. Threads')
    plt.legend()
    plt.grid(True)
    plt.ylim(0, 1.2) # Set y-limit to show just above 1.0
    plt.xticks([1, 2, 4, 8, 16, 32])
    
    output_file = 'Figure_3_Efficiency.png'
    plt.savefig(output_file)
    print(f"Generated {output_file}")
    plt.close()

def plot_static_vs_dynamic_speedup(csv_file, dataset_name, output_filename):
    if not os.path.exists(csv_file):
        print(f"Error: {csv_file} not found.")
        return

    df = pd.read_csv(csv_file)

    # Filter data
    static_data = df[df['Schedule'] == 'Static']
    dynamic_data = df[df['Schedule'] == 'Dynamic']

    plt.figure(figsize=(10, 6))

    # Plot Static
    plt.plot(static_data['Threads'], static_data['Speedup'], marker='o', label='Static Schedule')

    # Plot Dynamic
    plt.plot(dynamic_data['Threads'], dynamic_data['Speedup'], marker='s', label='Dynamic Schedule')

    # Ideal Speedup
    max_threads = static_data['Threads'].max()
    plt.plot([1, max_threads], [1, max_threads], 'k--', alpha=0.5, label='Ideal Speedup')

    plt.xlabel('Number of Threads')
    plt.ylabel('Speedup (S)')
    plt.title(f'Static vs. Dynamic Speedup - {dataset_name}')
    plt.legend()
    plt.grid(True)
    plt.xticks([1, 2, 4, 8, 16, 32])
    
    plt.savefig(output_filename)
    print(f"Generated {output_filename}")
    plt.close()

def generate_figure_4():
    plot_static_vs_dynamic_speedup('performance_results_1.csv', 'Dataset 1 (Large)', 'Figure_4_Static_vs_Dynamic_Speedup_DS1.png')
    plot_static_vs_dynamic_speedup('performance_results_2.csv', 'Dataset 2 (Small)', 'Figure_4_Static_vs_Dynamic_Speedup_DS2.png')

def generate_figure_5():
    file1 = 'performance_results_1.csv'
    file2 = 'performance_results_2.csv'
    
    if not os.path.exists(file1) or not os.path.exists(file2):
        print("Error: CSV files not found.")
        return

    df1 = pd.read_csv(file1)
    df2 = pd.read_csv(file2)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 6))

    # Helper function for plotting on a specific axis
    def plot_on_axis(ax, df, title):
        static_data = df[df['Schedule'] == 'Static']
        dynamic_data = df[df['Schedule'] == 'Dynamic']
        
        # Parallel Times
        ax.plot(static_data['Threads'], static_data['Parallel_Time_ms'], marker='o', label='Parallel (Static)')
        ax.plot(dynamic_data['Threads'], dynamic_data['Parallel_Time_ms'], marker='s', label='Parallel (Dynamic)')
        
        # Sequential Baseline
        seq_time = df['Sequential_Time_ms'].iloc[0]
        ax.axhline(y=seq_time, color='r', linestyle='--', label=f'Sequential ({seq_time:.1f}ms)')
        
        ax.set_xlabel('Number of Threads')
        ax.set_ylabel('Time (ms)')
        ax.set_title(title)
        ax.legend()
        ax.grid(True)
        ax.set_xticks([1, 2, 4, 8, 16, 32])

    plot_on_axis(ax1, df1, 'Dataset 1 (Large)')
    plot_on_axis(ax2, df2, 'Dataset 2 (Small)')

    plt.suptitle('Sequential vs. Parallel Execution Time Comparison')
    plt.tight_layout()
    
    output_file = 'Figure_5_Seq_vs_Par_Time.png'
    plt.savefig(output_file)
    print(f"Generated {output_file}")
    plt.close()

if __name__ == "__main__":
    generate_figure_1()
    generate_figure_2()
    generate_figure_3()
    generate_figure_4()
    generate_figure_5()
